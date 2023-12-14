#include "Classes.h"

Base64::Base64() {
    alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
}


void Encoder::EncodeTriplet(const string& triplet) {
        string result = "====";

    int k = triplet.at(0) >> 2;
    result.at(0) = alphabet.at(k);

    k = (triplet.at(0) & 3) << 4;
    k = k | (triplet.at(1) >> 4);
    result.at(1) = alphabet.at(k);

    k = (triplet.at(1) & 0xF) << 2;
    k = k | (triplet.at(2) >> 6);
    result.at(2) = alphabet.at(k);

    k = triplet.at(2) & 0x3F;
    result.at(3) = alphabet.at(k);

    coded += result;

}

void Encoder::EncodeDuplet(const string& duplet) {
        string result = "====";

    int k = duplet.at(0) >> 2;
    result.at(0) = alphabet.at(k);

    k = (duplet.at(0) & 3) << 4;
    k = k | (duplet.at(1) >> 4);
    result.at(1) = alphabet.at(k);

    k = (duplet.at(1) & 0xF) << 2;
    result.at(2) = alphabet.at(k);

    coded += result;

}

void Encoder::EncodeSymbol(const char& symbol) {
        string result = "====";

    int k = symbol >> 2;
    result.at(0) = alphabet.at(k);

    k = (symbol & 3) << 4;
    result.at(1) = alphabet.at(k);

    coded += result;

}

int Decoder::getPos(const char& symbol) {
    for (int i = 0; i < 64; i++) {
        if (alphabet.at(i) == symbol)
            return i;

    }

    return -1;
}

int Decoder::DecodeTriplet(const string& triplet) {
        string result = "---";

    int k = getPos(triplet.at(0));
    if (k == -1) {
        return 1;
    }

    result.at(0) = k << 2;
    k = getPos(triplet.at(1));
    if (k == -1) {
        return 2;
    }

    result.at(0) = result.at(0) | (k >> 4);
    result.at(1) = (k << 4) & 0xFF;
    k = getPos(triplet.at(2));
    if (k == -1) {
        return 3;
    }

    result.at(1) = result.at(1) | (k >> 2);
    result.at(2) = (k << 6) & 0xFF;
    k = getPos(triplet.at(3));
    if (k == -1) {
        return 4;
    }

    result.at(2) = result.at(2) | k;
    output += result;
    return 0;
}

int Decoder::DecodeDuplet(const string& duplet) {
        string result = "--";

    int k = getPos(duplet.at(0));
    if (k == -1) {
        return 1;
    }

    result.at(0) = k << 2;
    k = getPos(duplet.at(1));
    if (k == -1) {
        return 2;
    }

    result.at(0) = result.at(0) | (k >> 4);
    result.at(1) = (k << 4) & 0xFF;
    k = getPos(duplet.at(2));
    if (k == -1) {
        return 3;
    }
    result.at(1) = result.at(1) | k >> 2;


    output += result;
    return 0;

}

int Decoder::DecodeSymbol(const string& symbol) {
        string result = "-";

    int k = getPos(symbol.at(0));
    if (k == -1) {
        return 1;
    }

    result.at(0) = k << 2;
    k = getPos(symbol.at(1));
    if (k == -1) {
        return 2;
    }

    result.at(0) = result.at(0) | (k >> 4);

    output += result;
    return 0;
}


void Encoder::EncodeFile(const string& FilePath) {
    inputFile.open(FilePath);
    if (!inputFile.is_open()) {
        cerr << "Error opening file: " << FilePath << endl;
        return;
    }

    char buffer[3];
    while (inputFile.read(buffer, 3)) {
        string triplet(buffer, inputFile.gcount());
        EncodeTriplet(triplet);
    }


    string lastTriplet(buffer, inputFile.gcount());
    if (!lastTriplet.empty()) {
        if (lastTriplet.size() == 2) {
            EncodeDuplet(lastTriplet);
        }
        else if (lastTriplet.size() == 1) {
            EncodeSymbol(lastTriplet[0]);
        }
    }

    inputFile.close();
}


int Decoder::DecodeFile(const string& FilePath) {
    inputFile.open(FilePath);
    if (!inputFile.is_open()) {
        cerr << "Error opening file: " << FilePath << endl;
        return 1;
    }

    string line;
    while (getline(inputFile, line)) {
        if (!line.empty() && line[0] == '-') {
            continue;
        }

        size_t pos = 0;
        while (pos < line.size()) {
            char buffer[4];

            size_t readSize = min(line.size() - pos, sizeof(buffer));
            line.copy(buffer, readSize, pos);
            string quadruplet(buffer, readSize);
            string remaining(buffer, readSize);

            if (remaining[2] == '=' && remaining[3] == '=') {
                remaining.erase(3);
                remaining.erase(2);
                DecodeSymbol(remaining);
                break;
            }
            else if (remaining[3] == '=') {
                remaining.erase(3);
                DecodeDuplet(remaining);
                break;
            }

            int result = DecodeTriplet(quadruplet);
            if (result != 0) {
                cerr << "Error decoding quadruplet in file: " << FilePath << endl;
                inputFile.close();
                return result;
            }

            pos += readSize;
        }
    }

    inputFile.close();
    return 0;
}

void Encoder::DataToFile(const string& FilePath) {
    outputFile.open(FilePath);
    if (!outputFile.is_open()) {
        cerr << "Error opening file: " << FilePath << endl;
        return;
    }
    outputFile << coded;
    outputFile.close();
}

void Decoder::DataToFile(const string& FilePath) {
    outputFile.open(FilePath);
    if (!outputFile.is_open()) {
        cerr << "Error opening file: " << FilePath << endl;
        return;
    }
    outputFile << output;
    outputFile.close();
}

// Для того, щоб можна було зручно перевикористовувати програму

void Encoder::Clear_Coded() {
    coded.clear();
}

void Decoder::Clear_Output() {
    output.clear();
}


int main() {
    
    //Для першого файлу

    Encoder encoder;
    Decoder decoder;

    encoder.EncodeFile("100_bytes.txt");
    encoder.DataToFile("encoded_100.base64");
    encoder.Clear_Coded();


    int decodeResult = decoder.DecodeFile("encoded_100.base64");
    if (decodeResult == 0) {
        decoder.DataToFile("decoded_100.txt");
        decoder.Clear_Output();
    }
    else {
        cerr << "Decoding failed with error code: " << decodeResult << endl;
    }

    //Для другого файлу

    Encoder encoderr;
    Decoder decoderr;

    encoderr.EncodeFile("101_bytes.txt");
    encoderr.DataToFile("encoded_101.base64");
    encoderr.Clear_Coded();

    int decodeResultr = decoderr.DecodeFile("encoded_101.base64");
    if (decodeResultr == 0) {
        decoderr.DataToFile("decoded_101.txt");
        decoderr.Clear_Output();
    }
    else {
        cerr << "Decoding failed with error code: " << decodeResultr << endl;
    }

    //Для третього файлу

    Encoder encoders;
    Decoder decoders;

    encoders.EncodeFile("102_bytes.txt");
    encoders.DataToFile("encoded_102.base64");
    encoders.Clear_Coded();

    int decodeResults = decoders.DecodeFile("encoded_102.base64");
    if (decodeResults == 0) {
        decoders.DataToFile("decoded_102.txt");
        decoders.Clear_Output();
    }
    else {
        cerr << "Decoding failed with error code: " << decodeResults << endl;
    }

    return 0;
}