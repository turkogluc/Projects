package hw;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.*;

public class MainClass {
	
	public static List<String> casting;
	public static List<String> movies;
	public static int matrix[][];
	public static int distance[][];
	public static int prev[][];
	

	public static void print_path (int i, int j) {
		int k;
		while(i!=j) {
			// geriye dogru giderek iliskiler ekrana bastýrýlýyor.
			k = prev[i][j] ;
			System.out.println(casting.get(k) + "," + movies.get(matrix[k][j]-1) + " filminde " + casting.get(j) + " ile rol aldý ");
			j = k;
		}
		  
	}
	
	public static void main(String[] args) {
		
		BufferedReader br = null;
		FileReader fr = null;
		String FILENAME = "src/hw/test.txt";

		try {

			fr = new FileReader(FILENAME);
			br = new BufferedReader(fr);

			// ilk satir okunuyor
			String firstLine,sCurrentLine;
			firstLine = br.readLine();
			String[] tokens = firstLine.split("-");
			
			casting = new LinkedList<>();

			for (String t : tokens) {
				if(! t.equals("#")) {
					// oyuncu listesi olusturuluyor
					casting.add(t);
				}
			}
			
			int size = casting.size();
			matrix = new int[size][size];
			movies = new LinkedList<>();

			int movieCounter = 0;
			while ((sCurrentLine = br.readLine()) != null) {
				Vector<Integer> neighbours = new Vector<>();
				int tokenCounter = 0;
				String[] tok = sCurrentLine.split("-");
				for (String t : tok) {
					
					if(tokenCounter == 0) {
						movies.add(tok[0]);		// film listesi dolduruluyor.
						
					}else {
						if(t.equals("1")) {
							// bir filmin oyuncularý listeye ekleniyor
							neighbours.addElement(new Integer(tokenCounter-1));
						}
					}
					tokenCounter++;
				}
				
				// komsuluk matrisi burada doluyor.
				// ayný filmde oynayan oyuncular icin tabloda filmin index numarasý yazýlýr.
				for(int i=0;i<neighbours.size()-1;i++) {
					for(int j=i+1;j<neighbours.size();j++){
						// bir filmin oyuncularý arasýnda ki iliski komsuluk matrisine yazýlýyor. 
						// film indexi 1 den baslatilmistir.
						matrix[neighbours.get(i)][neighbours.get(j)] = movieCounter+1;
						matrix[neighbours.get(j)][neighbours.get(i)] = movieCounter+1;
					}
				}
				
				movieCounter++;
				neighbours.clear();
				
			}
			
			// Burada komsuluk matrisi ekrana bastýrýlýyor
			
			System.out.print("Komsuluk matrisi:\n");
			for(int[] raw : matrix) {
				for(int a: raw) {
					System.out.print(a + " ");
				}
				System.out.print("\n");
			}
			
			
			// # Floyd Warshall algoritmasi ile iki node arasý en kýsa mesafe bulunacak
			
			
			// burada uzaklýk matrisi olusturuluyor
			distance = new int[size][size] ;
			for (int i = 0; i<size; i++){
			     for (int j = 0; j<size; j++){
			    	if(i==j) {
			    		distance[i][j] = 0;
			    	}
			    	else if(matrix[i][j] != 0) {
			        	distance[i][j] = 1;
			        }else {
			        	// arada direk bag yoksa uzaklýk sonsuz yapýlmalý
			        	// ben buyuk bir deger girdim (99999)
			        	distance[i][j] = 99999;
			        }
			     } 

			}
			
			//yolu akýlda tutmak icin her node'dan bi önceki node prev matrisinde tutuluyor
			prev = new int[size][size]  ;
			for (int i = 0; i<prev.length; i++){
			     for (int j = 0; j<prev[i].length; j++){
			    	prev[i][j] = i ;
			     } 

			}
			
			int n = matrix.length;
			// Floyd Warshall algoritmasý
			for(int k=0; k < n ; k++ ) {
				for(int i=0; i < n ; i++ ) {
					for(int j=0; j < n ; j++ ) {
						if (distance[i][k] + distance[k][j] < distance[i][j]) {
							distance[i][j] = distance[i][k]+distance[k][j];
							prev[i][j] = prev[k][j];
					    }
					}
				}
			}
			
			// # Not : Oyuncu isimleri girilirken case-sensitive 
			
			Scanner scanner = new Scanner (System.in);
			String first;
			String second;
			System.out.print("\n");
			System.out.print("1. kisiyi giriniz: ");
			first = scanner.nextLine();
			System.out.print("2. kisiyi giriniz: ");
			second = scanner.nextLine();
			
			int firstIndex = casting.indexOf(first);
			int secondIndex = casting.indexOf(second);
			
			print_path(firstIndex,secondIndex);

		} catch (IOException e) {

			e.printStackTrace();

		}

		
	}

}
