INPUT = data/escola_artes_visuais.ppm
OUTPUT = output/escola_artes_visuais.ppm

sobel: sobel.c libimg.c
	gcc -o sobel sobel.c libimg.c -lm -w
	gcc -o sobelOpenMP -fopenmp sobelOpenMP.c libimg.c -lm -w
	gcc -o sobelThread sobelThread.c libimg.c -lm -pthread -w
	
clean:
	rm -f sobel
	rm -f sobelOpenMP
	rm -f sobelThread