#ifndef _JSONTOSPIFFS_h
#define _JSONTOSPIFFS_h

#include <FS.h>
#include <ArduinoJson.h>
#include <IPAddress.h>

#define JSONTOSPIFFS_RELEASE  // Comment to enable debug output

#define DBG_OUTPUT_PORT Serial

#ifndef JSONTOSPIFFS_RELEASE
#define DEBUGLOG(...) DBG_OUTPUT_PORT.printf(__VA_ARGS__)
#else
#define DEBUGLOG(...)
#endif

class JSONtoSPIFFS {
public:
	JSONtoSPIFFS();
	~JSONtoSPIFFS();
	
	void begin(FS* fs);

	bool loadConfigFile(const String name);
	bool saveConfigFile();
	bool closeConfigFile();
	bool deleteConfigFile(const String name);
	
	bool getValue(String key, String& value);
	bool setValue(String key, String value);
	bool getValue(String key, IPAddress& value);
	bool setValue(String key, IPAddress value);
	bool getValue(String key, char& value);
	bool setValue(String key, char value);
	bool getValue(String key, int& value);
	bool setValue(String key, int value);
	bool getValue(String key, long& value);
	bool setValue(String key, long value);
	bool getValue(String key, float& value);
	bool setValue(String key, float value);
	bool getValue(String key, bool& value);
	bool setValue(String key, bool value);

	bool removeKey(String key);

protected:
	FS* _fs;
	JsonObject* _json; //only valid as long as _jsonBuffer AND _buf are valid!
	StaticJsonBuffer<2000> _jsonBuffer; //Keeps the memory of the JsonObject
	std::unique_ptr<char[]> _buf; //Keeps the JsonString from the Json File
	String _filename;
	bool _JSONParsed;

	bool createConfigFile(const String filename);
	bool getValueVariant(String key, JsonVariant& value);
};

#endif // _JSONTOSPIFFS_h