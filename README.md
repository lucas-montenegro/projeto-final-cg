# Projeto final de computação gráfica
## Objetos do quarto
Foram criados os seguintes objetos para a cena:
* Quarto, cama, colchão, travesseiro, quadro e moldura, apoio para os pés, portas, maçanetas, janelas, cadeira, mesa, abajur, ventilador e lixeira.

## Movimentação e câmera
A movimentação se dá pelas teclas **w** (para frente na direção que você está olhando), **s** (para trás na direção que você está olhando), **espaço** (para cima) e **b** (para baixo).

A camêra simula um jogo de fps, onde é possível girar a camêra com o movimento do mouse, sendo limitado em 90° para cima e para baixo.

## Modelagem e textura
Os objetos foram modelados no blender (versão 2.92) junto da aplicação de textura nos objetos. Foi montada toda a cena com os objetos dentro do quarto e cada objeto foi exportado no formato **.obj**, deixando para o opengl o trabalho de importar os objetos e as texturas da cena.

## Compilação e execução
### Windows
No windows é necessária as bibliotecas **FREEGLUT**, **STB_IMAGE** e **GLM**.

Para compilar execute o seguinte comando:
```sh
g++ main.cpp -o main -I"path_to_libraries" -lfreeglut -lopengl32 -lglu32
```

Para executar, basta digitar o seguinte comando:
```sh
./main
```

### Linux
No linux é necessária as bibliotecas **LGL**, **LGLU**, **LGLUT**, **STB_IMAGE** e **GLM**.

Para compilar execute o seguinte comando:
```sh
g++ main.cpp -o main -lGL -lGLU -lglut -I ~path_to_glm
```

Para executar, basta digitar o seguinte comando:
```sh
./main
```

## Referências
Foram utilizadas as seguintes referências:
* <https://github.com/valeriojr/monitoria-cg> 
* <https://www.dca.ufrn.br/~ambj/opengl/>
* <https://learnopengl.com/Getting-started/Camera>
* <https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glLight.xml>

