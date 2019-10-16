#include "stdafx.h"
#include "PathHelper.h"

void PathHelper::GetPathFromFile(const std::string & filename)
{
	std::ifstream m_Stream;
	m_Stream.open(filename);

	if (m_Stream.fail()) {
		std::cerr << "Could not open Path file " << filename << '\n';
		return;
	}

	//clear before add new paths
	m_Paths.clear();

	while (!m_Stream.eof()) {
		std::string line;
		std::getline(m_Stream, line);

		if (line.find("<") != std::string::npos && line.find(">") != std::string::npos) {
			m_Paths.push_back(std::vector<DirectX::XMFLOAT3>());
		}
		else {
			//Extract Position(x,y)
			auto position = DirectX::XMFLOAT3();
			auto charPos = line.find(",");
			auto charPos2 = line.find(")");

			position.x = std::stof(line.substr(1, charPos - 1));
			position.z = std::stof(line.substr(charPos + 1, charPos2 - 1));
			m_Paths[m_Paths.size() - 1].push_back(position);
		}
	}

	m_Stream.close();
}
