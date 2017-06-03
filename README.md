# Sistemas Operacionais 1 - Projeto 1

# Como compilar o Algorítmo de Sobel

* Compilação: **gcc sobel.c libimg.c -o sobel -lm**
* Execução: **./sobel imgInput imgOutput**

### Utilizar o Script em PHP

`php makeSobel.php <imgInput> <imgOutput>` 

`php makeSobelOpen.php <imgInput> <imgOutput>` ** Executar o sobel em openMp **

imgInput e imgOutput são opcionais, se não forem fornecidos ele utiliza, **escola_artes_visuais.ppm**.

A saída vai ser uma imagem em *.ppm e outra em *.jpg que dá para ser diretamente visualisado no Cloud9.

# Versionamento no Cloud9

Para verificar as versões anteriores, é só clicar com o botão direito no código e selecionar a opção **File History**.

* Pode apertar Play para ver todas as mudanças da mais antiga para a mais recente.

# IMPORTANTE!

Invés de utilizar o printf tradicional do C, utilizar o PrintFlushed, criei esse método para dar printf e dar flush no stdout. O printf normal as vezes buga com processos.

Para evitar problemas utilizar o PrintFlushed. ( Os parâmetros são os mesmo do printf )


# Utilizando o GIT

**Essa informação só é útil para quem editar diretamente na IDE local e enviar via GIT ( Não recomendado, aqui possui próprio versionamento também )**

Manter a Máquina Virtual ligada quando estiver utilizando o GIT. Pois ela entra em IDLE depois de um tempo
e não sincroniza automaticamente.

Se preferir pode dar um **git pull** no **Bash**.
