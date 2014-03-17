#include <iostream>
#include <Qt/qdatastream.h>
#include <Qt/qfile.h>
#include <Qt/qiodevice.h>


int main()
{
    QFile testfile("test.mobi");

    testfile.open(QIODevice::ReadOnly);
    QDataStream inStream(&testfile);

    int testresult = 0;

    //goto pos 60
    testresult = inStream.skipRawData ( 60 ) ;

    if (testresult > 0 )
		printf( "skipped %d bytes\n", testresult);

     char * tmpchar = new  char[8];

    //read 8 bytes
    testresult = inStream.readRawData(tmpchar,8);

    if (testresult > 0 )
		printf( "skipped %d bytes\n", testresult);

    printf( "tmpchar : %s\n", tmpchar);

    if (strncmp(tmpchar,"BOOKMOBI",8) == 0)
        printf ( "got MOBI header\n");

    //goto pos 76

    testfile.seek(76);
    printf("at position %ld\n", testfile.pos());

    delete tmpchar;

    tmpchar = new  char[2];

    int records = 0 ;
    testresult = inStream.readRawData(tmpchar,2);
    if (testresult == 2) {
        records = tmpchar[0] <<8 | tmpchar[1] ;   //needs a batter way to figure it out
        printf ( "mobi records : %d \n", records);
    }

    printf("parsing record0\n");

    //records start at  78 and take up 8*records bytes of data
    //http://wiki.mobileread.com/wiki/PDB#Palm_Database_Format

    testfile.seek(78);

    delete tmpchar;

    tmpchar = new char[8];

    //4b - record offset from start of file
    //1b - record flags
    //3b - record id


    for (int i=0;i<records;i++) {
        printf("record %d\n", i);
        testresult = inStream.readRawData(tmpchar,8);
        unsigned char * tmpchar2 = (unsigned char*)tmpchar;
        if (testresult == 8) {

            for(int j = 0; j < 8; j++)
                printf("%02x ", (unsigned char)tmpchar2[j]);
            printf("\n");

            unsigned int offset = tmpchar2[3] | tmpchar2[0] <<24 | tmpchar2[1]<<16 | tmpchar2[2]<<8  ;   //needs a batter way to figure it out
            int flags = tmpchar2[4];
            unsigned int record_id = tmpchar2[5] << 16 | tmpchar2[6] << 8 | tmpchar2[7];
            printf ( "offset : %0x [%d] \n", offset, offset);
            printf ( "flags : %0x [%d] \n", flags, flags);
            printf ( "record_id : %0x [%d] \n", record_id, record_id);
        }
    }

    printf("at position %ld\n", testfile.pos());

    printf("jumping the 2 byte gap to actual data\n");
    testfile.seek(testfile.pos() + 2);
    printf("at position %ld\n", testfile.pos());


    //go through palmdocheader ; 16 bytes

    //go through MOBI header

    //go through EXTH header, if found (indicated in MOBI header)

    return 0;
}




