#include "GuiApp.hpp"

#include "Evaluator.hpp"
#include "ModelReporter.hpp"
#include "NaturalLanguageParser.hpp"
#include "SemanticTableau.hpp"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#ifdef _WIN32
#include <windows.h>
#include <gdiplus.h>
#include <mmsystem.h>
#endif

#include <array>
#include <algorithm>
#include <cstdio>
#include <cstdint>
#include <fstream>
#include <functional>
#include <string>
#include <vector>

namespace inferencias {

namespace {

#ifdef _WIN32
class AudioPlayer {
public:
    void playButton() const {
        play("assets/music/Boton.mp3", "inferencias_button");
    }

    void playAnalyze() const {
        play("assets/music/Analisar.mp3", "inferencias_analyze");
    }

    void playTransition() const {
        play("assets/music/Transicion.mp3", "inferencias_transition");
    }

private:
    static void play(const char* path, const char* alias) {
        char command[512]{};
        std::snprintf(command, sizeof(command), "close %s", alias);
        mciSendStringA(command, nullptr, 0, nullptr);
        const int opened = std::snprintf(
            command, sizeof(command),
            "open \"%s\" type mpegvideo alias %s", path, alias);
        if (opened < 0 || static_cast<std::size_t>(opened) >= sizeof(command) ||
            mciSendStringA(command, nullptr, 0, nullptr) != 0) {
            std::fprintf(stderr, "No se pudo abrir el audio: %s\n", path);
            return;
        }
        std::snprintf(command, sizeof(command), "play %s from 0", alias);
        if (mciSendStringA(command, nullptr, 0, nullptr) != 0) {
            std::fprintf(stderr, "No se pudo reproducir el audio: %s\n", path);
        }
    }
};

class BackgroundTexture {
public:
    bool load(const wchar_t* path) {
        Gdiplus::Bitmap bitmap(path, FALSE);
        if (bitmap.GetLastStatus() != Gdiplus::Ok) {
            return false;
        }

        Gdiplus::Rect bounds(
            0, 0, static_cast<INT>(bitmap.GetWidth()),
            static_cast<INT>(bitmap.GetHeight()));
        Gdiplus::BitmapData data{};
        if (bitmap.LockBits(
                &bounds, Gdiplus::ImageLockModeRead,
                PixelFormat32bppARGB, &data) != Gdiplus::Ok) {
            return false;
        }

        constexpr GLenum clampToEdge = 0x812F;
        constexpr GLenum bgra = 0x80E1;
        glGenTextures(1, &texture_);
        glBindTexture(GL_TEXTURE_2D, texture_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clampToEdge);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clampToEdge);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA,
            static_cast<GLsizei>(bitmap.GetWidth()),
            static_cast<GLsizei>(bitmap.GetHeight()), 0,
            bgra, GL_UNSIGNED_BYTE, data.Scan0);
        glBindTexture(GL_TEXTURE_2D, 0);
        bitmap.UnlockBits(&data);
        width_ = bitmap.GetWidth();
        height_ = bitmap.GetHeight();
        return true;
    }

    ~BackgroundTexture() {
        if (texture_ != 0) {
            glDeleteTextures(1, &texture_);
        }
    }

    GLuint id() const noexcept { return texture_; }
    int width() const noexcept { return width_; }
    int height() const noexcept { return height_; }

private:
    GLuint texture_{0};
    int width_{0};
    int height_{0};
};
#endif

constexpr std::size_t kMaxAssignments = 1048576;
constexpr std::size_t kInputCapacity = 512;

struct InputBuffer {
    std::array<char, kInputCapacity> data{};

    explicit InputBuffer(const std::string& value = {}) {
        std::snprintf(data.data(), data.size(), "%s", value.c_str());
    }

    std::string value() const {
        return data.data();
    }
};

void drawFormula(const std::string& title, const Formula::Ptr& formula) {
    ImGui::TextWrapped("%s: %s", title.c_str(), formula->toString().c_str());
}

void drawTableauNode(const std::shared_ptr<TableauNode>& node) {
    if (!node) {
        return;
    }
    const bool open = node->state == TableauNodeState::Open;
    const ImVec4 color = node->state == TableauNodeState::Closed
        ? ImVec4(0.95f, 0.35f, 0.35f, 1.0f)
        : open ? ImVec4(0.35f, 0.85f, 0.45f, 1.0f)
               : ImVec4(0.85f, 0.75f, 0.35f, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_Text, color);
    const std::string label = "Nodo " + std::to_string(node->id) +
        (open ? " [abierta]" : " [cerrada]");
    const bool expanded = ImGui::TreeNode(label.c_str());
    ImGui::PopStyleColor();
    if (!expanded) {
        return;
    }
    for (const auto& formula : node->formulas) {
        ImGui::BulletText("%s", formula->toString().c_str());
    }
    if (!node->expansion.empty()) {
        ImGui::TextWrapped("Regla: %s", node->expansion.c_str());
    }
    if (!node->closureReason.empty()) {
        ImGui::TextWrapped("%s", node->closureReason.c_str());
    }
    for (const auto& child : node->children) {
        drawTableauNode(child);
    }
    ImGui::TreePop();
}

struct GraphNode {
    std::shared_ptr<TableauNode> node;
    ImVec2 center;
    ImVec2 size;
};

struct GraphViewState {
    float zoom{1.0f};
    ImVec2 pan{0.0f, 0.0f};
};

ImVec4 tableauColor(TableauNodeState state) {
    if (state == TableauNodeState::Closed) {
        return ImVec4(0.55f, 0.12f, 0.12f, 1.0f);
    }
    if (state == TableauNodeState::Open) {
        return ImVec4(0.10f, 0.42f, 0.20f, 1.0f);
    }
    return ImVec4(0.42f, 0.34f, 0.10f, 1.0f);
}

void resetGraphView(GraphViewState& view) {
    view.zoom = 1.0f;
    view.pan = ImVec2(0.0f, 0.0f);
}

void drawTableauSummary(const std::shared_ptr<TableauNode>& root) {
    if (!root) {
        return;
    }
    std::size_t nodes = 0;
    std::size_t openBranches = 0;
    std::size_t closedBranches = 0;
    std::function<void(const std::shared_ptr<TableauNode>&)> visit =
        [&](const std::shared_ptr<TableauNode>& node) {
            ++nodes;
            if (node->children.empty()) {
                if (node->state == TableauNodeState::Open) {
                    ++openBranches;
                } else if (node->state == TableauNodeState::Closed) {
                    ++closedBranches;
                }
            }
            for (const auto& child : node->children) {
                visit(child);
            }
        };
    visit(root);
    ImGui::Text("Nodos: %zu", nodes);
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.35f, 0.9f, 0.5f, 1.0f),
                      "Ramas abiertas: %zu", openBranches);
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f),
                      "Ramas cerradas: %zu", closedBranches);
}

void drawTableauGraph(const std::shared_ptr<TableauNode>& root,
                      const char* childId, GraphViewState& view) {
    if (!root) {
        ImGui::TextUnformatted("No hay nodos para mostrar.");
        return;
    }

    constexpr float nodeDiameter = 154.0f;
    constexpr float nodeRadius = nodeDiameter / 2.0f;
    constexpr float levelHeight = 190.0f;
    constexpr float horizontalGap = 45.0f;
    constexpr float margin = 30.0f;
    std::vector<GraphNode> graphNodes;
    std::size_t leafCount = 0;
    std::size_t maxDepth = 0;

    std::function<std::size_t(const std::shared_ptr<TableauNode>&, std::size_t)>
        countLeaves = [&](const std::shared_ptr<TableauNode>& node,
                          std::size_t depth) {
            maxDepth = std::max(maxDepth, depth);
            if (node->children.empty()) {
                return std::size_t{1};
            }
            std::size_t count = 0;
            for (const auto& child : node->children) {
                count += countLeaves(child, depth + 1);
            }
            return count;
        };
    leafCount = countLeaves(root, 0);

    std::size_t nextLeaf = 0;
    std::function<float(const std::shared_ptr<TableauNode>&, std::size_t)>
        place = [&](const std::shared_ptr<TableauNode>& node,
                    std::size_t depth) {
            float x = 0.0f;
            if (node->children.empty()) {
                x = static_cast<float>(nextLeaf++) *
                (nodeDiameter + horizontalGap);
            } else {
                std::vector<float> childX;
                for (const auto& child : node->children) {
                    childX.push_back(place(child, depth + 1));
                }
                for (const float value : childX) {
                    x += value;
                }
                x /= static_cast<float>(childX.size());
            }

            graphNodes.push_back({
                node,
                ImVec2(margin + x + nodeRadius,
                       margin + static_cast<float>(depth) * levelHeight),
                ImVec2(nodeDiameter, nodeDiameter)
            });
            return x;
        };
    place(root, 0);

    const float canvasWidth = std::max(
        900.0f, margin * 2.0f +
            static_cast<float>(leafCount) * (nodeDiameter + horizontalGap));
    const float canvasHeight = margin * 2.0f +
        static_cast<float>(maxDepth + 1) * levelHeight;
    const float scaledWidth = canvasWidth * view.zoom;
    const float scaledHeight = canvasHeight * view.zoom;
    ImGui::BeginChild(childId, ImVec2(0, 390), true,
                      ImGuiWindowFlags_HorizontalScrollbar);
    const ImVec2 canvasStart = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton("tableau_canvas",
                           ImVec2(scaledWidth, scaledHeight));
    const bool hovered = ImGui::IsItemHovered();
    if (hovered && ImGui::GetIO().MouseWheel != 0.0f) {
        view.zoom = std::clamp(view.zoom + ImGui::GetIO().MouseWheel * 0.1f,
                               0.45f, 2.0f);
    }
    if (hovered && ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 0.0f)) {
        view.pan.x += ImGui::GetIO().MouseDelta.x;
        view.pan.y += ImGui::GetIO().MouseDelta.y;
    }
    const ImVec2 origin(
        canvasStart.x + view.pan.x, canvasStart.y + view.pan.y);
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    const auto scalePoint = [&](const ImVec2& point) {
        return ImVec2(origin.x + point.x * view.zoom,
                      origin.y + point.y * view.zoom);
    };

    const ImU32 branchColor = IM_COL32(180, 185, 195, 255);
    for (const auto& graphNode : graphNodes) {
        if (graphNode.node->children.empty()) {
            continue;
        }

        const ImVec2 parentBottom = scalePoint(ImVec2(
            graphNode.center.x,
            graphNode.center.y + nodeRadius));
        std::vector<ImVec2> childTops;
        for (const auto& child : graphNode.node->children) {
            const auto childIt = std::find_if(
                graphNodes.begin(), graphNodes.end(),
                [&child](const GraphNode& candidate) {
                    return candidate.node == child;
                });
            if (childIt != graphNodes.end()) {
                childTops.push_back(scalePoint(ImVec2(
                    childIt->center.x,
                    childIt->center.y - nodeRadius)));
            }
        }
        if (childTops.empty()) {
            continue;
        }

        const float junctionY = parentBottom.y + 24.0f * view.zoom;
        const ImVec2 junction(parentBottom.x, junctionY);
        const float thickness = 3.0f * view.zoom;
        drawList->AddBezierCubic(
            parentBottom,
            ImVec2(parentBottom.x, parentBottom.y + 10.0f * view.zoom),
            ImVec2(junction.x, junction.y - 10.0f * view.zoom),
            junction, branchColor, thickness);

        if (childTops.size() == 1) {
            const ImU32 childColor = ImGui::ColorConvertFloat4ToU32(
                tableauColor(graphNode.node->children.front()->state));
            drawList->AddBezierCubic(
                junction,
                ImVec2(junction.x, junction.y + 12.0f * view.zoom),
                ImVec2(childTops.front().x, childTops.front().y -
                    12.0f * view.zoom),
                childTops.front(), childColor, thickness);
            continue;
        }

        drawList->AddCircleFilled(junction, 4.0f * view.zoom, branchColor);

        for (std::size_t index = 0; index < childTops.size(); ++index) {
            const ImVec2& childTop = childTops[index];
            const ImU32 childColor = ImGui::ColorConvertFloat4ToU32(
                tableauColor(graphNode.node->children[index]->state));
            const float direction = childTop.x < junction.x ? -1.0f : 1.0f;
            const ImVec2 branchPoint(
                junction.x + (childTop.x - junction.x) * 0.42f,
                junction.y - 2.0f * view.zoom);
            drawList->AddBezierCubic(
                junction,
                ImVec2(junction.x + direction * 24.0f * view.zoom,
                       junction.y + 8.0f * view.zoom),
                ImVec2(childTop.x - direction * 24.0f * view.zoom,
                       childTop.y - 28.0f * view.zoom),
                childTop, childColor, thickness);
            drawList->AddCircleFilled(branchPoint, 2.5f * view.zoom,
                                      childColor);
            const auto& childFormulas = graphNode.node->children[index]->formulas;
            if (!childFormulas.empty() && view.zoom >= 0.65f) {
                const std::string label = childFormulas.back()->toString();
                drawList->AddText(
                    ImVec2(branchPoint.x + 6.0f * view.zoom,
                           junctionY - 18.0f * view.zoom),
                    IM_COL32(225, 225, 225, 255), label.c_str());
            }
        }
    }

    for (const auto& graphNode : graphNodes) {
        const ImVec2 center = scalePoint(graphNode.center);
        const float radius = nodeRadius * view.zoom;
        const ImVec4 color = tableauColor(graphNode.node->state);
        const ImU32 fillColor = ImGui::ColorConvertFloat4ToU32(color);
        drawList->AddCircleFilled(center, radius, fillColor, 48);
        drawList->AddCircle(center, radius, IM_COL32(225, 225, 225, 255),
                            48, 2.0f * view.zoom);

        const std::string title = "Nodo " +
            std::to_string(graphNode.node->id) + "  " +
            (graphNode.node->state == TableauNodeState::Closed
                 ? "CERRADA"
                 : graphNode.node->state == TableauNodeState::Open
                     ? "ABIERTA"
                     : "EXPANDIDA");
        const auto drawCentered = [&](const std::string& text, float y,
                                      ImU32 textColor) {
            const ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
            drawList->AddText(
                ImVec2(center.x - textSize.x * 0.5f,
                       center.y + y * view.zoom),
                textColor, text.c_str());
        };
        drawCentered(title, -45.0f, IM_COL32(255, 255, 255, 255));

        const std::size_t visibleFormulas = std::min<std::size_t>(
            graphNode.node->formulas.size(), 4);
        for (std::size_t index = 0; index < visibleFormulas; ++index) {
            drawCentered(graphNode.node->formulas[index]->toString(),
                         -22.0f + static_cast<float>(index) * 17.0f,
                         IM_COL32(240, 240, 240, 255));
        }
        if (!graphNode.node->expansion.empty()) {
            drawCentered("Regla: " + graphNode.node->expansion, 48.0f,
                         IM_COL32(255, 235, 145, 255));
        }
        if (!graphNode.node->closureReason.empty()) {
            const ImVec2 first = ImVec2(
                center.x - radius * 0.55f, center.y - radius * 0.55f);
            const ImVec2 second = ImVec2(
                center.x + radius * 0.55f, center.y + radius * 0.55f);
            const ImVec2 third = ImVec2(
                center.x + radius * 0.55f, center.y - radius * 0.55f);
            const ImVec2 fourth = ImVec2(
                center.x - radius * 0.55f, center.y + radius * 0.55f);
            drawList->AddLine(first, second, IM_COL32(255, 215, 215, 255),
                              5.0f * view.zoom);
            drawList->AddLine(third, fourth, IM_COL32(255, 215, 215, 255),
                              5.0f * view.zoom);
        }
    }
    ImGui::EndChild();
}

bool writeDot(const std::string& path, const std::string& dot) {
    std::ofstream output(path);
    output << dot;
    return static_cast<bool>(output);
}

} // namespace

class GuiState {
public:
    std::vector<InputBuffer> premiseInputs{InputBuffer{}};
    InputBuffer queryInput;
    NaturalLanguageParser parser;
    std::vector<Formula::Ptr> premises;
    Formula::Ptr query;
    std::vector<std::string> premiseTexts;
    std::string error;
    std::string status;
    std::string report;
    std::string queryTableauText;
    SemanticTableau premiseTableau;
    SemanticTableau queryTableau;
    GraphViewState premiseGraphView;
    GraphViewState queryGraphView;
    ModelSearchResult models;
    EntailmentResult entailment;
    bool analyzed{false};

    void clearResults() {
        premises.clear();
        query.reset();
        premiseTexts.clear();
        error.clear();
        status.clear();
        report.clear();
        queryTableauText.clear();
        premiseTableau.clear();
        queryTableau.clear();
        resetGraphView(premiseGraphView);
        resetGraphView(queryGraphView);
        models = {};
        entailment = {};
        analyzed = false;
    }

    void loadExample() {
        premiseInputs.clear();
        premiseInputs.emplace_back("Si llueve, entonces llevo paraguas.");
        premiseInputs.emplace_back("Está lloviendo.");
        queryInput = InputBuffer("Llevo paraguas.");
        clearResults();
    }

    void analyze() {
        clearResults();
        parser = NaturalLanguageParser{};
        for (const auto& input : premiseInputs) {
            const std::string text = input.value();
            const ParseResult result = parser.parse(text);
            if (!result.ok()) {
                error = "Error en una premisa: " + result.error;
                return;
            }
            premiseTexts.push_back(text);
            premises.push_back(result.formula);
        }
        const std::string queryText = queryInput.value();
        if (!queryText.empty()) {
            const ParseResult result = parser.parse(queryText);
            if (!result.ok()) {
                error = "Error en la consulta: " + result.error;
                return;
            }
            query = result.formula;
        }

        for (const auto& premise : premises) {
            premiseTableau.addPremise(premise);
        }
        premiseTableau.build();
        writeDot("tableau_premisas.dot", premiseTableau.toDot());

        const Evaluator evaluator;
        models = evaluator.enumerateModels(premises, kMaxAssignments);
        const ModelReporter reporter;
        report = reporter.formatModels(models, parser.atomExtractor().atoms());

        if (query) {
            queryTableau = SemanticTableau{};
            for (const auto& premise : premises) {
                queryTableau.addPremise(premise);
            }
            queryTableau.addPremise(
                Formula::unary(Connective::Not, query));
            queryTableau.build();
            queryTableauText = queryTableau.toText();
            writeDot("tableau_consulta.dot", queryTableau.toDot());
            entailment = evaluator.checkEntailment(
                premises, *query, kMaxAssignments);
            report += reporter.formatEntailment(
                entailment, parser.atomExtractor().atoms());
        }
        analyzed = true;
        status = "Analisis completado.";
    }
};

int GuiApp::run() {
    if (glfwInit() == GLFW_FALSE) {
        std::fprintf(stderr, "No se pudo inicializar GLFW.\n");
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(
        1280, 800, "Motor de Inferencias - Logica Proposicional", nullptr,
        nullptr);
    if (!window) {
        glfwTerminate();
        std::fprintf(stderr, "No se pudo crear la ventana GLFW.\n");
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

#ifdef _WIN32
    Gdiplus::GdiplusStartupInput gdiplusInput;
    ULONG_PTR gdiplusToken = 0;
    const bool gdiplusReady =
        Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusInput, nullptr) ==
        Gdiplus::Ok;
#endif

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 10.0f;
    style.ChildRounding = 8.0f;
    style.FrameRounding = 6.0f;
    style.Colors[ImGuiCol_WindowBg].w = 0.82f;
    style.Colors[ImGuiCol_ChildBg].w = 0.64f;
    style.Colors[ImGuiCol_FrameBg].w = 0.82f;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

#ifdef _WIN32
    BackgroundTexture background;
    if (gdiplusReady) {
        background.load(L"assets/images/Fondo.jpg");
    }
#endif

    GuiState state;
    AudioPlayer audio;
    int activeTab = 0;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

#ifdef _WIN32
        if (background.id() != 0) {
            int framebufferWidth = 0;
            int framebufferHeight = 0;
            glfwGetFramebufferSize(
                window, &framebufferWidth, &framebufferHeight);
            const float imageAspect =
                static_cast<float>(background.width()) /
                static_cast<float>(background.height());
            const float viewportAspect =
                static_cast<float>(framebufferWidth) /
                static_cast<float>(framebufferHeight);
            ImVec2 min(0.0f, 0.0f);
            ImVec2 max(
                static_cast<float>(framebufferWidth),
                static_cast<float>(framebufferHeight));
            if (imageAspect > viewportAspect) {
                const float visibleWidth =
                    static_cast<float>(framebufferHeight) * imageAspect;
                min.x = (static_cast<float>(framebufferWidth) -
                    visibleWidth) * 0.5f;
                max.x = min.x + visibleWidth;
            } else {
                const float visibleHeight =
                    static_cast<float>(framebufferWidth) / imageAspect;
                min.y = (static_cast<float>(framebufferHeight) -
                    visibleHeight) * 0.5f;
                max.y = min.y + visibleHeight;
            }
            ImGui::GetBackgroundDrawList()->AddImage(
                static_cast<ImTextureID>(background.id()),
                min, max, ImVec2(0, 0), ImVec2(1, 1),
                IM_COL32(255, 255, 255, 180));
        }
#endif

        ImGui::Begin("Motor de Inferencias - Logica Proposicional");
        if (ImGui::BeginTabBar("main_tabs")) {
            if (ImGui::BeginTabItem("Entrada de datos")) {
                if (activeTab != 0) {
                    audio.playTransition();
                    activeTab = 0;
                }
                ImGui::TextWrapped(
                    "Escriba premisas de la gramatica limitada en espanol.");
                if (ImGui::Button("Agregar premisa")) {
                    state.premiseInputs.emplace_back();
                    audio.playButton();
                }
                ImGui::SameLine();
                if (ImGui::Button("Eliminar ultima")) {
                    audio.playButton();
                    if (state.premiseInputs.size() > 1) {
                        state.premiseInputs.pop_back();
                    }
                }
                for (std::size_t index = 0;
                     index < state.premiseInputs.size(); ++index) {
                    ImGui::PushID(static_cast<int>(index));
                    ImGui::InputText(
                        ("Premisa " + std::to_string(index + 1)).c_str(),
                        state.premiseInputs[index].data.data(),
                        state.premiseInputs[index].data.size());
                    ImGui::PopID();
                }
                ImGui::InputText("Consulta", state.queryInput.data.data(),
                                 state.queryInput.data.size());
                if (ImGui::Button("Analizar")) {
                    audio.playAnalyze();
                    state.analyze();
                }
                ImGui::SameLine();
                if (ImGui::Button("Limpiar")) {
                    audio.playButton();
                    state.premiseInputs = {InputBuffer{}};
                    state.queryInput = InputBuffer{};
                    state.clearResults();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cargar ejemplo")) {
                    audio.playButton();
                    state.loadExample();
                }
                if (!state.error.empty()) {
                    ImGui::TextColored(
                        ImVec4(1, 0.35f, 0.35f, 1), "%s",
                        state.error.c_str());
                }
                if (!state.status.empty()) {
                    ImGui::TextColored(
                        ImVec4(0.35f, 1, 0.45f, 1), "%s",
                        state.status.c_str());
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Atomos y formulas")) {
                if (activeTab != 1) {
                    audio.playTransition();
                    activeTab = 1;
                }
                if (!state.analyzed) {
                    ImGui::TextUnformatted("Analice un caso primero.");
                } else {
                    const int modelColumns = state.models.models.empty()
                        ? 0
                        : static_cast<int>(state.models.models.size());
                    if (ImGui::BeginTable("atoms", 3 + modelColumns,
                                          ImGuiTableFlags_Borders |
                                          ImGuiTableFlags_RowBg)) {
                        ImGui::TableSetupColumn("Simbolo");
                        ImGui::TableSetupColumn("Proposicion");
                        ImGui::TableSetupColumn("ID");
                        for (int model = 0; model < modelColumns; ++model) {
                            ImGui::TableSetupColumn(
                                ("Modelo " + std::to_string(model + 1)).c_str());
                        }
                        ImGui::TableHeadersRow();
                        for (const auto& atom : state.parser.atomExtractor().atoms()) {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::TextUnformatted(atom.symbol.c_str());
                            ImGui::TableNextColumn();
                            ImGui::TextWrapped("%s", atom.sentence.c_str());
                            ImGui::TableNextColumn();
                            ImGui::Text("%llu",
                                static_cast<unsigned long long>(atom.id));
                            for (const auto& model : state.models.models) {
                                ImGui::TableNextColumn();
                                const auto value = model.find(atom.symbol);
                                ImGui::TextUnformatted(
                                    value != model.end()
                                        ? (value->second ? "V" : "F")
                                        : "-");
                            }
                        }
                        ImGui::EndTable();
                    }
                    ImGui::Separator();
                    for (std::size_t index = 0;
                         index < state.premises.size(); ++index) {
                        drawFormula(
                            "Premisa " + std::to_string(index + 1),
                            state.premises[index]);
                    }
                    if (state.query) {
                        drawFormula("Consulta", state.query);
                    }
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Arbol semantico")) {
                if (activeTab != 2) {
                    audio.playTransition();
                    activeTab = 2;
                }
                if (!state.analyzed) {
                    ImGui::TextUnformatted("Analice un caso primero.");
                } else {
                    ImGui::TextUnformatted("Tableau de premisas");
                    ImGui::TextUnformatted(
                        "Verde: rama abierta | Rojo: rama cerrada | "
                        "Amarillo: nodo pendiente o expandido");
                    drawTableauSummary(state.premiseTableau.root());
                    ImGui::SameLine();
                    ImGui::Text("Zoom: %.0f%%", state.premiseGraphView.zoom * 100.0f);
                    if (ImGui::Button("Zoom +")) {
                        audio.playButton();
                        state.premiseGraphView.zoom = std::min(
                            2.0f, state.premiseGraphView.zoom + 0.1f);
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Zoom -")) {
                        audio.playButton();
                        state.premiseGraphView.zoom = std::max(
                            0.45f, state.premiseGraphView.zoom - 0.1f);
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Restablecer vista")) {
                        audio.playButton();
                        resetGraphView(state.premiseGraphView);
                    }
                    ImGui::TextUnformatted(
                        "Rueda: zoom | Arrastre central: desplazar");
                    drawTableauGraph(state.premiseTableau.root(),
                                     "premise_tableau_graph",
                                     state.premiseGraphView);
                    ImGui::Separator();
                    ImGui::TextUnformatted("Detalle textual");
                    drawTableauNode(state.premiseTableau.root());
                    if (state.query) {
                        ImGui::Separator();
                        ImGui::TextUnformatted(
                            "Tableau de premisas y negacion de la consulta");
                        drawTableauSummary(state.queryTableau.root());
                        if (ImGui::Button("Restablecer vista de consulta")) {
                            audio.playButton();
                            resetGraphView(state.queryGraphView);
                        }
                        drawTableauGraph(state.queryTableau.root(),
                                         "query_tableau_graph",
                                         state.queryGraphView);
                        ImGui::TextUnformatted("Detalle textual");
                        drawTableauNode(state.queryTableau.root());
                    }
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Modelos y conclusion")) {
                if (activeTab != 3) {
                    audio.playTransition();
                    activeTab = 3;
                }
                if (!state.analyzed) {
                    ImGui::TextUnformatted("Analice un caso primero.");
                } else {
                    ImGui::BeginChild("report", ImVec2(0, 0), true);
                    ImGui::TextWrapped("%s", state.report.c_str());
                    ImGui::EndChild();
                }
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::End();

        ImGui::Render();
        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClearColor(0.07f, 0.08f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
#ifdef _WIN32
    if (gdiplusReady) {
        Gdiplus::GdiplusShutdown(gdiplusToken);
    }
#endif
    return 0;
}

} // namespace inferencias
