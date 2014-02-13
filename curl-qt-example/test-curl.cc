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


void readEntry(QXmlStreamReader &xml) {
	Q_ASSERT(xml.isStartElement() && xml.name() == "entry");
	printf(" -- entry -- \n");
	
	QXmlStreamReader::TokenType tmptoken;
	
	//while (xml.readNextStartElement()) { 
	
	while ((tmptoken = xml.readNext()) && xml.name() != "entry") {
		QXmlStreamAttributes attributes = xml.attributes();

		if (xml.name() == "title") {
			xml.readNext();  //skip to text value
			QString text = xml.text().toString();

			if (!text.trimmed().isEmpty()) {
				std::cout << "title : " 
					<< text.toStdString() ;
			}
		}

		if (xml.name() == "content" ) {
			xml.readNext();  //get to text value

			if(xml.tokenType() != QXmlStreamReader::Characters) {
				printf(" not characters \n");
			}

			QString text = xml.text().toString();

			if (!text.trimmed().isEmpty()) {
				std::cout << " content : " 
					<< text.toStdString() ;
			}
		}

		if (xml.name() == "link") {
			QString url = attributes.value("href").toString();

			if (!url.isEmpty()) {
				std::cout
					<< " url : " 
					<< url.toStdString() ;
			}
			std::cout << std::endl;
		}
	}

}

int main()
{
	curl_global_init( CURL_GLOBAL_ALL );
	CURL * myHandle;

	CURLcode result; // We’ll store the result of CURL’s webpage retrieval, for simple error checking.
	myHandle = curl_easy_init ( ) ;

	// Notice the lack of major error checking, for brevity

	//curl_easy_setopt(myHandle, CURLOPT_URL, "http://localhost:8080/opds");
	curl_easy_setopt(myHandle, CURLOPT_URL, "http://localhost:8080/opds/navcatalog/4e736572696573");

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
			//only entry elements hold interesting data
			if (xml.name() == "entry") { 
				readEntry(xml);
			}
		}
	}

	return 0;
}




