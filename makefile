CXX := c++
EXE := bin/AsteroidsB
ASTEROIDS_EXE := Asteroids
ASTEROIDS_SRC := src/Asteroids.cpp src/Ventana.cpp src/Nave.cpp src/Misil.cpp src/Margen.cpp src/Punto.cpp src/Puntaje.cpp src/Vida.cpp src/Asteroide.cpp src/Oportunidad.cpp src/VidaExtra.cpp src/EscudoItem.cpp src/TablaDePuntaje.cpp src/DobleDisparoItem.cpp src/Explosion.cpp

.PHONY: all clean AsteroidsB runAsteroids Nave runNave runAsteroids run

all: run

run: runAsteroids

runAsteroids: $(ASTEROIDS_EXE)
	./$(ASTEROIDS_EXE)

$(ASTEROIDS_EXE): $(ASTEROIDS_SRC)
	$(CXX) $(ASTEROIDS_SRC) -o $(ASTEROIDS_EXE) -std=c++17 -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

AsteroidsB: $(EXE)
	./$(EXE)

runAsteroidsB: $(EXE)
	./$(EXE)

$(EXE): src/AsteroidsB.cpp | bin
	$(CXX) $< -o $@ -std=c++17 -lsfml-audio -lsfml-system

bin:
	mkdir bin

clean:
	del /Q bin\*.exe

NAVE_EXE := bin/Nave

.PHONY: Nave runNave

Nave: $(NAVE_EXE)
	./$(NAVE_EXE)

runNave: $(NAVE_EXE)
	./$(NAVE_EXE)

$(NAVE_EXE): src/Nave.cpp | bin
	$(CXX) $< -o $@ -std=c++17 -lsfml-graphics -lsfml-window -lsfml-system
