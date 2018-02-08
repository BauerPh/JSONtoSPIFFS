#include "JSONtoSPIFFS.h"
#include <IPAddress.h>

JSONtoSPIFFS::JSONtoSPIFFS() {}
JSONtoSPIFFS::~JSONtoSPIFFS() {
	this->closeConfigFile();
	_fs = NULL;
}

void JSONtoSPIFFS::begin(FS* fs) {
	_fs = fs;
	if (!_fs) _fs->begin();
}

bool JSONtoSPIFFS::loadConfigFile(const String name) {
	_filename = name;
	if (!_filename.endsWith(".json")) _filename = _filename + ".json";
	if (!_filename.startsWith("/")) _filename = "/" + _filename;

	//Check if file exists
	if (!_fs->exists(_filename)) {
		DEBUGLOG("JSONtoSPIFFS::loadConfigFile - Config file doesn't exists, try to create...\r\n");
		if (!createConfigFile(_filename)) {
			DEBUGLOG("JSONtoSPIFFS::loadConfigFile - Failed to create config file\r\n");
			return false;
		}
	}
	//Open file
	File configFile = _fs->open(_filename, "r");
	if (!configFile) {
		DEBUGLOG("JSONtoSPIFFS::loadConfigFile - Failed to open config file");
		configFile.close();
		return false;
	}
	//allocate Buffer
	size_t size = configFile.size();
    _buf.reset(new char[size]);
	//Read File & close it
	configFile.readBytes(_buf.get(), size);
	configFile.close();
	//Parse JSON
	_json = &(_jsonBuffer.parseObject(_buf.get()));

	if (!_json->success()) {
		DEBUGLOG("JSONtoSPIFFS::loadConfigFile - Failed to parse config file\r\n");
		return false;
	}
	_JSONParsed = true;
	return true;
}

bool JSONtoSPIFFS::saveConfigFile() {
	if (!_JSONParsed) {
		DEBUGLOG("JSONtoSPIFFS::saveConfigFile - Config file not opened\r\n");
		return false;
	}
	File configFile = _fs->open(_filename, "w");
	if (!configFile) {
		DEBUGLOG("JSONtoSPIFFS::saveConfigFile - Failed to open config file for saving\r\n");
		configFile.close();
		return false;
	}
	_json->printTo(configFile);
	configFile.flush();
	configFile.close();
	//clear JSON Buffer to reuse it => !!!_json is invalid now!!!
	_jsonBuffer.clear();
	_buf.reset();
	_JSONParsed = false;
	return true;
}

bool JSONtoSPIFFS::closeConfigFile() {
	//clear JSON Buffer to reuse it => !!!_json is invalid now!!!
	if (!_JSONParsed) {
		DEBUGLOG("JSONtoSPIFFS::closeConfigFile - Config file not opened\r\n");
		return false;
	}
	_jsonBuffer.clear();
	_buf.reset();
	_JSONParsed = false;
	return true;
}

bool JSONtoSPIFFS::deleteConfigFile(const String name) {
	String tmpFilename;
	tmpFilename = name;
	if (!tmpFilename.endsWith(".json")) tmpFilename = tmpFilename + ".json";
	if (!tmpFilename.startsWith("/")) tmpFilename = "/" + tmpFilename;
	//Check if actual config file is opened
	if ((tmpFilename == _filename) && _JSONParsed) {
		DEBUGLOG("JSONtoSPIFFS::deleteConfigFile - Config file is opened...\r\n");
		return false;
	}
	//Check if file exists
	if (!_fs->exists(_filename)) {
		DEBUGLOG("JSONtoSPIFFS::deleteConfigFile - Config file doesn't exists...\r\n");
		return false;
	}
	//Delete file
	return _fs->remove(tmpFilename);
}

bool JSONtoSPIFFS::setValue(String key, bool value) {
	if (!_JSONParsed) {
		DEBUGLOG("JSONtoSPIFFS::setValue - Config file not opened\r\n");
		return false;
	}
	return _json->set(key, value);
}

bool JSONtoSPIFFS::getValue(String key, bool& value) {
	JsonVariant tmpValue;
	if (!getValueVariant(key, tmpValue)) return false;
	value = tmpValue.as<bool>();
	return true;
}

bool JSONtoSPIFFS::setValue(String key, char value) {
	if (!_JSONParsed) {
		DEBUGLOG("JSONtoSPIFFS::setValue - Config file not opened\r\n");
		return false;
	}
	return _json->set(key, (signed char)value);
}

bool JSONtoSPIFFS::getValue(String key, char& value) {
	JsonVariant tmpValue;
	if (!getValueVariant(key, tmpValue)) return false;
	value = char(tmpValue.as<signed char>());
	return true;
}

bool JSONtoSPIFFS::setValue(String key, int value) {
	if (!_JSONParsed) {
		DEBUGLOG("JSONtoSPIFFS::setValue - Config file not opened\r\n");
		return false;
	}
	return _json->set(key, (signed int)value);
}

bool JSONtoSPIFFS::getValue(String key, int& value) {
	JsonVariant tmpValue;
	if (!getValueVariant(key, tmpValue)) return false;
	value = int(tmpValue.as<signed int>());
	return true;
}

bool JSONtoSPIFFS::setValue(String key, long value) {
	if (!_JSONParsed) {
		DEBUGLOG("JSONtoSPIFFS::setValue - Config file not opened\r\n");
		return false;
	}
	return _json->set(key, (signed long)value);
}

bool JSONtoSPIFFS::getValue(String key, long& value) {
	JsonVariant tmpValue;
	if (!getValueVariant(key, tmpValue)) return false;
	value = long(tmpValue.as<signed long>());
	return true;
}

bool JSONtoSPIFFS::setValue(String key, float value) {
	if (!_JSONParsed) {
		DEBUGLOG("JSONtoSPIFFS::setValue - Config file not opened\r\n");
		return false;
	}
	return _json->set(key, value);
}

bool JSONtoSPIFFS::getValue(String key, float& value) {
	JsonVariant tmpValue;
	if (!getValueVariant(key, tmpValue)) return false;
	value = tmpValue.as<float>();
	return true;
}

bool JSONtoSPIFFS::setValue(String key, String value) {
	if (!_JSONParsed) {
		DEBUGLOG("JSONtoSPIFFS::setValue - Config file not opened\r\n");
		return false;
	}
	return _json->set(key, value);
}

bool JSONtoSPIFFS::getValue(String key, String& value) {
	JsonVariant tmpValue;
	if (!getValueVariant(key, tmpValue)) return false;
	value = tmpValue.as<String>();
	return true;
}

bool JSONtoSPIFFS::setValue(String key, IPAddress value) {
	if (!_JSONParsed) {
		DEBUGLOG("JSONtoSPIFFS::setValue - Config file not opened\r\n");
		return false;
	}
	//key already exist?
	JsonVariant tmpValue;
	if (getValueVariant(key, tmpValue)) {
		(*_json)[key][0] = value[0];
		(*_json)[key][1] = value[1];
		(*_json)[key][2] = value[2];
		(*_json)[key][3] = value[3];
		return true;
	} else {
		bool tmpBool;
		JsonArray& jsonArray = _json->createNestedArray(key);
		if (!jsonArray.success()) {
			DEBUGLOG("JSONtoSPIFFS::setValue - Error creating nested Array\r\n");
			return false;
		}
		tmpBool = jsonArray.add(value[0]);
		tmpBool &= jsonArray.add(value[1]);
		tmpBool &= jsonArray.add(value[2]);
		tmpBool &= jsonArray.add(value[3]);
		return tmpBool;
	}
}

bool JSONtoSPIFFS::getValue(String key, IPAddress& value) {
	JsonVariant tmpValue;
	if (!getValueVariant(key, tmpValue)) return false;
	JsonArray& tmpJsonArray = tmpValue.as<JsonArray>();
	value = IPAddress(tmpJsonArray.get<unsigned char>(0), tmpJsonArray.get<unsigned char>(1), tmpJsonArray.get<unsigned char>(2), tmpJsonArray.get<unsigned char>(3));
	return true;
}

bool JSONtoSPIFFS::removeKey(String key) {
	if (!_JSONParsed) {
		DEBUGLOG("JSONtoSPIFFS::getValue - Config file not opened\r\n");
		return false;
	}
	_json->remove(key);
	return true;
}



//********************************** PRIVATE FUCTIONS **********************************
bool JSONtoSPIFFS::createConfigFile(const String filename) {
	File configFile = _fs->open(filename, "w");
	if (!configFile) {
		DEBUGLOG("JSONtoSPIFFS::createConfigFile - Failed to open config file for creating\r\n");
		configFile.close();
		return false;
	}
	//create blank json file
	configFile.print("{}");
	configFile.flush();
	configFile.close();
	return true;
}

bool JSONtoSPIFFS::getValueVariant(String key, JsonVariant& value) {
	if (!_JSONParsed) {
		DEBUGLOG("JSONtoSPIFFS::getValueVariant - Config file not opened\r\n");
		return false;
	}
	JsonVariant tmpValue;
	tmpValue = _json->get<JsonVariant>(key);
	if (!tmpValue.success()) {
		DEBUGLOG("JSONtoSPIFFS::getValueVariant - Key doesn't exist\r\n");
		return false;
	}
	value = tmpValue;
	return true;
}