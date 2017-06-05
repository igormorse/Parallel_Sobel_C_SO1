# Intruções de Compilação

Comandos que deverão ser executados no Bash.

* Compile: make
* Clean: make clean

# Instruções de Execução

**Sobel com Processos**

`.\sobel <imgInput> <imgOutput> <numWorkers ( Opcional, Default = 4 )>`

**Sobel com Threads**

`.\sobelThread <imgInput> <imgOutput> <numWorkers ( Opcional, Default = 4 )>`

**Sobel com Threads utilizando OpenMP**

`.\sobelOpenMP <imgInput> <imgOutput> <numWorkers ( Opcional, Default = 4 )>`