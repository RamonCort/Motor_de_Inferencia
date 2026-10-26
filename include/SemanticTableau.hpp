#ifndef MOTOR_INFERENCIAS_SEMANTIC_TABLEAU_HPP
#define MOTOR_INFERENCIAS_SEMANTIC_TABLEAU_HPP

#include "Formula.hpp"

#include <memory>
#include <string>
#include <vector>

namespace inferencias {

enum class TableauNodeState {
    Pending,
    Expanded,
    Open,
    Closed
};

struct TableauNode {
    std::size_t id{0};
    std::vector<Formula::Ptr> formulas;
    TableauNodeState state{TableauNodeState::Pending};
    std::string expansion;
    std::string closureReason;
    std::vector<std::shared_ptr<TableauNode>> children;
};

struct TableauBranch {
    std::vector<Formula::Ptr> formulas;
    bool open{true};
    std::vector<std::string> history;
};

class SemanticTableau {
public:
    void addPremise(Formula::Ptr premise);
    void clear();
    void build();

    const std::vector<Formula::Ptr>& premises() const noexcept;
    const std::shared_ptr<TableauNode>& root() const noexcept;
    const std::vector<TableauBranch>& branches() const noexcept;
    bool isSatisfiable() const noexcept;
    bool isBuilt() const noexcept;

    std::string toText() const;
    std::string toDot() const;

private:
    std::vector<Formula::Ptr> premises_;
    std::shared_ptr<TableauNode> root_;
    std::vector<TableauBranch> branches_;
    bool built_{false};
    std::size_t nextNodeId_{1};
};

} // namespace inferencias

#endif
