#include <curl/curl.h>
#include <Qt/qstring.h>
#include <Qt/qxmlstream.h>
#include <iostream>

QString data;

size_t readData( char *ptr, size_t size, size_t nmemb, void *userdata) {

	//haven't yet figured out how to pass to *userdata without losing the pointer. ho hum.
	//this is a viable workaround

	data = QString( static_cast<char*>( ptr ) );

	return size*nmemb;
};

int main()
{
	curl_global_init( CURL_GLOBAL_ALL );
	CURL * myHandle;
	
	CURLcode result; // We’ll store the result of CURL’s webpage retrieval, for simple error checking.
	myHandle = curl_easy_init ( ) ;
	
	// Notice the lack of major error checking, for brevity

	curl_easy_setopt(myHandle, CURLOPT_URL, "http://localhost:8080/opds");

	curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, readData);

	curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &data);
	
	result = curl_easy_perform( myHandle );


	curl_easy_cleanup( myHandle );
	
	std::cout << data.toStdString() << std::endl;

	//try to get through xml

	if (!data.isEmpty()) {
		QXmlStreamReader xml(data);

		QXmlStreamReader::TokenType token;


		while ((token = xml.readNext()) != QXmlStreamReader::Invalid) {

			if (xml.name().toString() == "link") {
				printf(" >> link \n");
				QXmlStreamAttributes attributes = xml.attributes();
				QString url = attributes.value("href").toString();
				QString type = attributes.value("type").toString();

				if (!url.isEmpty()) {

					std::cout 	<< " url " 
						<< url.toStdString() 
						<< std::endl 
						<< " type " 
						<< type.toStdString() << std::endl;

					std::cout 	<< " name "
						<< xml.name().toString().toStdString() 
						<< std::endl;
				}
				printf( " << link \n");
			}

			if (xml.name().toString() == "content" ) {
				printf(" >> content \n");
				QString elementName = xml.name().toString();
				xml.readNext();
				if(xml.tokenType() != QXmlStreamReader::Characters) {
					printf(" not characters \n");
					return 0;
				}
				
				QString text = xml.text().toString();
				
				if (!text.isEmpty()) {

					std::cout 
						<< "  text : " 
						<< text.toStdString() 
						<< " x\n   "
						<< std::endl;
				}
				printf(" << content \n");
			}
			printf(" next \n");
		}
	}
	
	return 0;
}




