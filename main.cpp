#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

struct Header {
    struct Pixel {
        unsigned char red;
        unsigned char blue;
        unsigned char green;

        Pixel() {}
    };
    char  idLength;
    char  colorMapType;
    char  dataTypeCode;
    short colorMapOrigin;
    short colorMapLength;
    char  colorMapDepth;
    short xOrigin;
    short yOrigin;
    short width;
    short height;
    char  bitsPerPixel;
    char  imageDescriptor;
    unsigned int pixelNum;
    vector<Pixel> vectorPixels;

    Header() {}

};

Header::Pixel pixel;
Header finalHeader;

Header ChannelRedBlue(Header &header) {
    for (unsigned i = 0; i < header.vectorPixels.size(); i++) {
        unsigned int value = header.vectorPixels[i].red * 4;
        if (value > 255) {
            header.vectorPixels[i].red= 255;
        }
        else header.vectorPixels[i].red *= 4;
        header.vectorPixels[i].blue = 0;
    }
    return header;
}

Header ChannelGreen(Header &header) {

    for (unsigned i = 0; i < header.vectorPixels.size(); i++) {
        unsigned int value = header.vectorPixels[i].green + 200;
        if (value > 255) {
            header.vectorPixels[i].green = 255;
        }
        else header.vectorPixels[i].green += 200;
    }
    return header;

}

unsigned char Clamp(unsigned char color1, unsigned char color2) {
    //unsigned char color;
    if (color2 > color1 ) {
        return 0;
    }
    else{
        return color1-color2;
    }
}

Header Combine(Header &h1, Header &h2, Header &h3) {
    Header finalHeader = h1;

    for (unsigned i = 0; i <h1.vectorPixels.size(); i++) {
        finalHeader.vectorPixels[i].red = h1.vectorPixels[i].red;
        finalHeader.vectorPixels[i].green = h2.vectorPixels[i].green;
        finalHeader.vectorPixels[i].blue = h3.vectorPixels[i].blue;
    }
    return finalHeader;
}

Header Flip(Header &header) {
    Header temp = header;

    for (unsigned i = header.vectorPixels.size()-1, j = 0; j < header.vectorPixels.size(); i--, j++) {
        header.vectorPixels[i].red = temp.vectorPixels[j].red;
        header.vectorPixels[i].green = temp.vectorPixels[j].green;
        header.vectorPixels[i].blue = temp.vectorPixels[j].blue;
    }


    return header;
}

Header Multiply(Header &header1, Header &header2) {
    Header finalHeader;
    finalHeader.idLength = header1.idLength;
    finalHeader.colorMapType = header1.colorMapType;
    finalHeader.dataTypeCode = header1.dataTypeCode;
    finalHeader.colorMapOrigin = header1.colorMapOrigin;
    finalHeader.colorMapLength = header1.colorMapLength;
    finalHeader.colorMapDepth = header1.colorMapDepth;
    finalHeader.xOrigin = header1.xOrigin;
    finalHeader.yOrigin = header1.yOrigin;
    finalHeader.width = header1.width;
    finalHeader.height = header1.height;
    finalHeader.bitsPerPixel = header1.bitsPerPixel;
    finalHeader.imageDescriptor = header1.imageDescriptor;
    finalHeader.pixelNum = header1.pixelNum;

    for (unsigned i = 0; i < header1.vectorPixels.size(); i++) {
        finalHeader.vectorPixels.push_back(pixel);
        finalHeader.vectorPixels[i].blue = ((header1.vectorPixels[i].blue * header2.vectorPixels[i].blue) / 255.0f)+.5f;
        finalHeader.vectorPixels[i].green = ((header1.vectorPixels[i].green * header2.vectorPixels[i].green)/255.0f)+.5f;
        finalHeader.vectorPixels[i].red = ((header1.vectorPixels[i].red * header2.vectorPixels[i].red)/255.0f)+.5f;

    }

    return finalHeader;
}

Header Overlay(Header &header1, Header &header2) {
    Header finalHeader = header1;
    float r1 = 0, g1 = 0, b1 = 0, r2 = 0, g2 = 0, b2 = 0;
    float r = 0, b = 0, g = 0;
    for (unsigned i = 0; i < header1.vectorPixels.size(); i++) {

        b1 = (header1.vectorPixels[i].blue / 255.0f);
        b2 = (header2.vectorPixels[i].blue / 255.0f);
        g1 = (header1.vectorPixels[i].green / 255.0f);
        g2 = (header2.vectorPixels[i].green / 255.0f);
        r1 = (header1.vectorPixels[i].red / 255.0f);
        r2 = (header2.vectorPixels[i].red / 255.0f);

        if (b2 <= .5) {
            b = (float) 2 * b1 * b2;
        }
        else b = (float) 1 -2*(1-b1)*(1-b2);
        if (g2 <= .5) {
            g = (float) 2 * g1 * g2;
        }
        else g = (float) 1 -2*(1-g1)*(1-g2);
        if (r2 <= .5) {
            r = (float) 2 * r1 * r2;
        }
        else r = (float) 1 -2*(1-r1)*(1-r2);

        finalHeader.vectorPixels[i].blue = (b * 255)+.5f;
        finalHeader.vectorPixels[i].green = (g * 255)+.5f;
        finalHeader.vectorPixels[i].red = (r * 255)+.5f;
    }
    return finalHeader;
}

Header Subtract(Header &header1, Header &header2) {
    Header finalHeader;
    finalHeader = header1;
    for (unsigned i = 0; i <header1.vectorPixels.size(); i++) {

        finalHeader.vectorPixels[i].blue = (header2.vectorPixels[i].blue - header1.vectorPixels[i].blue);
        finalHeader.vectorPixels[i].green = header2.vectorPixels[i].green - header1.vectorPixels[i].green;
        finalHeader.vectorPixels[i].red = header2.vectorPixels[i].red - header1.vectorPixels[i].red;

        finalHeader.vectorPixels[i].blue = Clamp(header2.vectorPixels[i].blue, header1.vectorPixels[i].blue);
        finalHeader.vectorPixels[i].green = Clamp(header2.vectorPixels[i].green, header1.vectorPixels[i].green);
        finalHeader.vectorPixels[i].red = Clamp(header2.vectorPixels[i].red, header1.vectorPixels[i].red);

    }

    cout << "checking subtract..." << endl;
    //need to clamp to min or max values of the data type after the operation?
    //however, to avoid the overflow/underflow issue, you might need to perform the calculation
    // in a data type that can store a larger range (like an integer),
    //then clamp and reassign to another variable afterward.
    return finalHeader;
}

Header Screen(Header &header1, Header &header2) {
    Header finalHeader;
    finalHeader = header1;
    float r1 = 0, g1 = 0, b1 = 0, r2 = 0, g2 = 0, b2 = 0;
    float r = 0, b = 0, g = 0;
    for (unsigned i = 0; i < header1.vectorPixels.size(); i++) {

        b1 = (header1.vectorPixels[i].blue/255.0f);
        b2 = (header2.vectorPixels[i].blue/255.0f);
        g1 = (header1.vectorPixels[i].green/255.0f);
        g2 = (header2.vectorPixels[i].green/255.0f);
        r1 = (header1.vectorPixels[i].red/255.0f);
        r2 = (header2.vectorPixels[i].red/255.0f);

        b = (float)(1.0 - (1.0-b1) * (1.0-b2));
        g = (float)(1.0 - (1.0-g1) * (1.0-g2));
        r = (float)(1.0 - (1.0-r1) * (1.0-r2));

        finalHeader.vectorPixels[i].blue = (b * 255)+.5f;
        finalHeader.vectorPixels[i].green = (g * 255)+.5f;
        finalHeader.vectorPixels[i].red = (r * 255)+.5f;

    }
    return finalHeader;

}

//functions to write to channels
Header Red(Header &header) {
    for (unsigned i = 0; i < header.vectorPixels.size(); i++) {
        header.vectorPixels[i].blue = header.vectorPixels[i].red;
        header.vectorPixels[i].green = header.vectorPixels[i].red;
    }
    return header;
}
Header Green(Header &header) {
    for (unsigned i = 0; i < header.vectorPixels.size(); i++) {
        header.vectorPixels[i].red = header.vectorPixels[i].green;
        header.vectorPixels[i].blue = header.vectorPixels[i].green;
    }
    return header;
}

Header Blue(Header &header) {
    for (unsigned i = 0; i < header.vectorPixels.size(); i++) {
        header.vectorPixels[i].red = header.vectorPixels[i].blue;
        header.vectorPixels[i].green = header.vectorPixels[i].blue;
    }
    return header;
}

///////********************////////////////////


/***********************************************************/

Header WriteFile(string filename, Header &header) {
    ofstream file (filename, ios::binary);
    if (file.is_open()) {
        file.write(&header.idLength, 1);
        file.write(&header.colorMapType, 1);
        file.write(&header.dataTypeCode, 1);
        file.write((char *) &header.colorMapOrigin, 2);
        file.write((char *) &header.colorMapLength, 2);
        file.write(&header.colorMapDepth, 1);
        file.write((char *)&header.xOrigin, 2);
        file.write((char *)&header.yOrigin, 2);
        file.write((char *)&header.width, 2);
        file.write((char *)&header.height, 2);
        file.write(&header.bitsPerPixel, 1);
        file.write(&header.imageDescriptor, 1);

        for (unsigned i = 0; i < header.vectorPixels.size(); i++) {
            file.write((char *) &header.vectorPixels[i].blue, 1);
            file.write((char *) &header.vectorPixels[i].green, 1);
            file.write((char *) &header.vectorPixels[i].red, 1);
        }

        file.close();
        return header;
    }

}

Header ReadFile(string filename) {
    ifstream file(filename, ios::binary);
    Header headerObject;
    if (file.is_open()) {
        file.read(&headerObject.idLength, 1);
        file.read(&headerObject.colorMapType, 1);
        file.read(&headerObject.dataTypeCode, 1);
        file.read((char *) &headerObject.colorMapOrigin, 2);
        file.read((char *) &headerObject.colorMapLength, 2);
        file.read(&headerObject.colorMapDepth, 1);
        file.read((char *)&headerObject.xOrigin, 2);
        file.read((char *)&headerObject.yOrigin, 2);
        file.read((char *)&headerObject.width, 2);
        file.read((char *)&headerObject.height, 2);
        file.read(&headerObject.bitsPerPixel, 1);
        file.read(&headerObject.imageDescriptor, 1);

        headerObject.pixelNum = (unsigned int)(headerObject.width * headerObject.height);

        //Image data
        for (unsigned i = 0; i < headerObject.pixelNum; i++) {

            headerObject.vectorPixels.push_back(pixel);
            file.read((char*)&headerObject.vectorPixels[i].blue, 1);
            file.read((char*)&headerObject.vectorPixels[i].green, 1);
            file.read((char*)&headerObject.vectorPixels[i].red, 1);

        }

        file.close();
        return headerObject;
    }
}


void printCheck(Header header1) {
    cout << "idLength: " << (int)header1.idLength << endl;
    cout << "Color Map Type:  " << (int)header1.colorMapType << endl;
    cout << "Data Type Code:  " << header1.dataTypeCode << endl;
    cout << "Color Map origin: " << header1.colorMapOrigin<< endl;
    cout << "colorMapLength:  " << header1.colorMapLength << endl;
    cout << "color map depth: " << (int)header1.colorMapDepth << endl;
    cout << "x origin: " << header1.xOrigin << endl;
    cout << "y origin: " << header1.yOrigin << endl;
    cout << "width: " << (int)header1.width << endl;
    cout << "height: " << (int)header1.height << endl;
    cout << "pixel depth: " << (int)header1.bitsPerPixel << endl;
    cout << "image descriptor: " << (int)header1.imageDescriptor << endl;
}

bool Test(Header &example, Header &test) {
    //bool whatsGoingOn = true;

    if (example.idLength != test.idLength || example.colorMapType != test.colorMapType || example.dataTypeCode != test.dataTypeCode) {
        return false;
    }

    else if (example.colorMapOrigin != test.colorMapOrigin || example.colorMapLength != test.colorMapLength || example.colorMapDepth != test.colorMapDepth) {
        return false;
    }

    else if (example.xOrigin != test.xOrigin || example.yOrigin != test.yOrigin || example.width != test.width || example.height != test.height) {
        return false;
    }

    else if (example.bitsPerPixel != test.bitsPerPixel || example.imageDescriptor != test.imageDescriptor) {
        return false;
    }
    else if (example.vectorPixels.size() != test.vectorPixels.size()) {
        return false;
    }
    else if (example.vectorPixels.size() == test.vectorPixels.size()) {
        for (unsigned i = 0; i < example.vectorPixels.size(); i++) {
            if (example.vectorPixels[i].red != test.vectorPixels[i].red ||
                example.vectorPixels[i].blue != test.vectorPixels[i].blue ||
                example.vectorPixels[i].green != test.vectorPixels[i].green) {
                cout << "Expected red for " << i << ": " << (int) example.vectorPixels[i].red << endl;
                cout << "My Red: " << (int) test.vectorPixels[i].red << endl;

                cout << "Expected blue: " << (int) example.vectorPixels[i].blue << endl;
                cout << "my blue: " << (int) test.vectorPixels[i].blue << endl;

                cout << "expected green: " << (int) example.vectorPixels[i].green << endl;
                cout << "my green: " << (int) test.vectorPixels[i].green << endl;

                return false;

            }
        }
    }
    return true;

}

int main() {
    Header finalHeader, header1, header2, header3, example, example2;

    // Part 1
    cout << "part 1" << endl;
    string filename1 = "input//layer1.tga";
    header1 = ReadFile(filename1);
    //printCheck(header1);
    string filename2 = "input//pattern1.tga";
    header2 = ReadFile(filename2);
    //multiply
    finalHeader = Multiply(header1, header2);
    finalHeader = WriteFile("output//part1.tga", finalHeader);
    example = ReadFile("examples//EXAMPLE_part1.tga");
    bool a = Test(example,finalHeader);
    if (a == 1) {
        cout << "Part 1: Success!" << endl;
    }
    else {
        cout << "at least u tried" << endl;

    }

// Part 2
    cout << "test 2" << endl;
    string filename3 = "input//layer2.tga";
    string filename4 = "input//car.tga";
    header3 = ReadFile(filename3);
    Header header4 = ReadFile(filename4);
    Header finalHeader2 = Subtract(header3, header4);
    finalHeader2 = WriteFile("output//part2.tga", finalHeader2);
    example2 = ReadFile("examples//EXAMPLE_part2.tga");
    bool b = Test(example2,finalHeader2);
    if (b == 1) {
        cout << "Part 2: Success!" << endl;
    }
    else { cout << "oof" << endl;
    }

//Part 3
    cout << "part 3" << endl;
    Header temp, header5, header6, finalHeader3, example3;
    string filename5 = "input//layer1.tga";
    header5 = ReadFile(filename5);
    string filename6 = "input//pattern2.tga";
    header6 = ReadFile(filename6);
    temp = Multiply(header5, header6);
    finalHeader3 = ReadFile("input//text.tga");
    finalHeader3 = Screen(finalHeader3, temp);
    finalHeader3 = WriteFile("output//part3.tga", finalHeader3);
    example3 = ReadFile("examples//EXAMPLE_part3.tga");
    bool c = Test(example3, finalHeader3);
    if (c ==1) {
        cout << "Part 3: Success!" << endl;
    }
    else {
        cout << "nope buddy sorry" << endl;
    }


    cout << "testing part 4.." << endl;
    //Part 4
    Header header7, header8, finalHeader4, temp1, temp2, example4;
    string filename7 = "input//layer2.tga";
    string filename8 = "input//circles.tga";
    header7 = ReadFile(filename7);
    header8 = ReadFile(filename8);
    temp1 = Multiply(header7, header8);
    temp2 = ReadFile("input//pattern2.tga");
    finalHeader4 = Subtract(temp2, temp1);
    finalHeader4 = WriteFile("output//part4.tga", finalHeader4);
    example4 = ReadFile("examples//EXAMPLE_part4.tga");
    bool d = Test(example4, finalHeader4);
    if (d == 1) {
        cout << "Part 4: Success!" << endl;
    }
    else {
        cout << "nah fam" << endl;
    }


    cout << "part 5" << endl; //Part 5
    Header header9, header10, finalHeader5, example5;
    string filename9 = "input//layer1.tga";
    string filename10 = "input//pattern1.tga";
    header9 = ReadFile(filename9);
    header10 = ReadFile(filename10);
    finalHeader5 = Overlay(header9, header10);
    finalHeader5 = WriteFile("output//part5.tga", finalHeader5);
    example5 = ReadFile("examples//EXAMPLE_part5.tga");
    bool e = Test(example5, finalHeader5);
    if (e == 1) {
        cout << "Part 5: Success!" << endl;
    }
    else {
        cout << "spaghetti" << endl;
    }


    cout << "part 6" << endl; //Part 6
    Header header11, finalHeader6, example6;
    header11 = ReadFile("input//car.tga");
    finalHeader6 = ChannelGreen(header11);
    finalHeader6 = WriteFile("output//part6.tga", finalHeader6);
    example6 = ReadFile("examples//EXAMPLE_part6.tga");
    bool f = Test(example6, finalHeader6);
    if (f == 1) {
        cout << "Part 6: Success!" << endl;
    }
    else {
        cout << "rip" << endl;
    }


    cout << "part 7" << endl; //Part 7
    Header header12, example7;
    header12 = ReadFile("input//car.tga");
    header12 = ChannelRedBlue(header12);
    header12 = WriteFile("output//part7.tga", header12);
    example7 = ReadFile("examples//EXAMPLE_part7.tga");
    bool g = Test(example7, header12);
    if (g == 1) {
        cout << "Part 7: Success!" << endl;
    }
    else {
        cout << "uh oh spaghetti-o's" << endl;
    }


    cout << "part 8 " << endl;
    Header header13, red, blue, green, example8, ex2, ex3;
    header13 = ReadFile("input//car.tga");
    red = Red(header13);
    red = WriteFile("output//part8_r.tga", red);
    example8 = ReadFile("examples//EXAMPLE_part8_r.tga");
    bool h = Test(example8, red);
    if (h == 1) {
        cout << "Part 8 (red): Success!" << endl;
    }
    else {
        cout << "c'est triste" << endl;
    }
    cout << endl;

    green = ReadFile("input//car.tga");
    green = Green(green);
    green = WriteFile("output//part8_g.tga", green);
    ex2 = ReadFile("examples//EXAMPLE_part8_g.tga");
    bool i = Test(ex2, green);
    if (i == 1) {
        cout << "Part 8 (green): Success!" << endl;
    }
    else cout << ":/" << endl;
    cout << endl;

    blue = ReadFile("input//car.tga");
    blue = Blue(blue);
    blue = WriteFile("output//part8_b.tga", blue);
    ex3 = ReadFile("examples//EXAMPLE_part8_b.tga");
    bool j = Test(ex3, blue);
    if (j == 1) {
        cout << "Part 8 (blue): Success!" << endl;
    }
    else {
        cout << ":((( yike" << endl;
    }


    cout << "pt 9 " << endl;
    Header red1, green1, blue1, finalHeader9, example9;
    red1 = ReadFile("input//layer_red.tga");
    green1 = ReadFile("input//layer_green.tga");
    blue1 = ReadFile("input//layer_blue.tga");
    finalHeader9 = Combine(red1, green1, blue1);
    finalHeader9 = WriteFile("output//part9.tga", finalHeader9);
    example9 = ReadFile("examples//EXAMPLE_part9.tga");
    bool l = Test(example9, finalHeader9);
    if (l == 1) {
        cout << "Part 9: Success!" << endl;
    }
    else {
        cout << "oof oh no" << endl;
    }


    cout << "part 10" << endl;
    Header header14, finalHeader10, example10;
    string flnm = "input//text2.tga";
    header14 = ReadFile(flnm);
    header14 = Flip(header14);
    finalHeader10 = WriteFile("output//part10.tga", header14);
    example10 = ReadFile("examples//EXAMPLE_part10.tga");
    bool z = Test(example10, finalHeader10);
    if (z == 1) {
        cout << "Part 10: Success!" << endl;
    }
    else {
        cout << "this ain't it chief" << endl;
    }


    return 0;
}
