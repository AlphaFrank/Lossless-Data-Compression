#include<iostream>
#include<string>
#include<list>
#include<bitset>
#include<fstream>
#include<deque>
#include<ctime>

// A class that read specific number of bits
class bitReader{
	// A buffer
	std::list<int> queue;

public:
	// Constructor
	bitReader() {}

	// Read specific number of bits to get an int
	int readInt(int numberOfBits){
		// Check if the buffer has enough bits
		checkReadyToInput(numberOfBits);

		std::bitset<14> bs;

		// Read that many bits
		for (int i=numberOfBits-1; i>=0; i--){
			bs[i] = queue.front();
			queue.pop_front();
		}

		long result = bs.to_ulong();

		// Return the int
		return (int) result;
	}

	// Read a char
	char readChar(){
		// Check if the buffer has enough bits
		checkReadyToInput(8);

		std::bitset<8> bs;

		// Read 8 bits
		for (int i=7; i>=0; i--){
			bs[i] = queue.front();
			queue.pop_front();
		}

		long result = bs.to_ulong();

		// Return the char
		return (char) result;
	}

private:
	void checkReadyToInput(int numberOfBits){
		// If the buffer does not contain that many bits, read from stdin
		while (queue.size() < numberOfBits){

			char input;
			std::cin.get(input);
			std::bitset<8> bs(input);

			for (int i=7; i>=0; i--){
				queue.push_back(bs[i]);
			}
		}
	}

};



int main(int argc, char* argv[]){
	// Start timing
	std::clock_t start = std::clock();

	// Initialize parameters
	int N;
	int L;
	int S;
	std::string inputFileName;
	std::ifstream in;

	// If a input file name given, direct the stdin to that file
	if (argc == 2){
		inputFileName = argv[1];
		in.open(inputFileName);
		if (!in.good()){
			std::cerr << "Input file does not exist." << std::endl;
			return 0;
		}
		std::cin.rdbuf(in.rdbuf());
	}

	// Initialize a bitreader
	bitReader br = bitReader();

	// Read the first three parameters
	N = br.readInt(8);
	L = br.readInt(8);
	S = br.readInt(8);

	// Declare some useful parameters
	int code;
	int strLength;
	int offset;
	std::deque<char> window;
	char c;

	// Keep read until it encounter <0,0>
	while (true){
		// Read the code
		code = br.readInt(L);
		if (code == 0){
			// Read S bits
			strLength = br.readInt(S);
			// If <0,0>, stop
			if (strLength == 0){
				break;
			}
			// Else output chars
			else{
				for(int i=0; i<strLength; i++){
					c = br.readChar();
					std::cout << c;
					window.push_back(c);

				}
			}
		} else {
			code++;
			offset = br.readInt(N);
			// Output by using double token
			for (int i=0; i<code; i++){
				std::cout << window[window.size()-offset];
				window.push_back(window[window.size()-offset]);
			}
		}
	}





	// Make sure the input file is closed
	if (argc == 2){
		in.close();
	}

	// Print out the running time
	double duration = (std::clock()-start) / (double) CLOCKS_PER_SEC;
	std::cerr << "Decompressing duration: " << duration << " seconds" << std::endl;



	return 0;
}
