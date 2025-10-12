#include <string>


enum DataTypes {

} : char;

class Packet {
public:
	Packet() {

	}
	
	std::string getStrData() {

	}

	bool setStrData() {

	}

	DataTypes getType() {
		return m_type;
	}

	void setType(DataTypes type) {
		m_type = type;
	}

	~Packet() {

	}
private:
	DataTypes m_type;
};