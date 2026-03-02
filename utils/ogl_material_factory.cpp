#include "ogl_material_factory.hpp"
#include <iostream>
#include <ranges>
#include <variant>
#include <algorithm>
#include <array>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"


inline ShaderProgramFiles listShaderFiles(const fs::path& aShaderDir) {
	if (!fs::exists(aShaderDir) || !fs::is_directory(aShaderDir)) {
		throw std::runtime_error("Shader dir path is not a directory or does not exist.");
	}
	std::cout << "Loading shaders from directory: " << aShaderDir << "\n";

	std::map<std::string, std::regex> patterns = {
		{ "include", std::regex("(.*)\\.include\\.glsl") },
		{ "vertex", std::regex("(.*)\\.vertex\\.glsl") },
		{ "fragment", std::regex("(.*)\\.fragment\\.glsl") },
		{ "geometry", std::regex("(.*)\\.geometry\\.glsl") },
		{ "control", std::regex("(.*)\\.control\\.glsl") },
		{ "evaluation", std::regex("(.*)\\.evaluation\\.glsl") },
		{ "compute", std::regex("(.*)\\.compute\\.glsl") },
		{ "program", std::regex("(.*)\\.program") },
	};
	ShaderProgramFiles shaderFiles;

	for (const auto& entry : fs::directory_iterator(aShaderDir)) {
		if (!entry.is_regular_file()) {
			std::cout << "Skipping " << entry << "\n";
			continue;
		}
		std::string filename = entry.path().filename().string();
		std::smatch matches;

		for (const auto &pattern : patterns) {
			if (std::regex_match(filename, matches, pattern.second) && matches.size() == 2) {
				std::cout << "Found shader file: " << filename << "\n";
				shaderFiles[pattern.first][matches[1]] = entry;
			}
		}
	}
	return shaderFiles;
}

inline std::map<std::string, std::string> parseProgramFile(const fs::path& filePath) {
	std::map<std::string, std::string> parsedData;
	std::ifstream file(filePath);

	if (!file.is_open()) {
		throw OpenGLError("Failed to open program file: " + filePath.string());
	}

	std::string line;
	while (std::getline(file, line)) {
		auto pos = line.find(':');
		if (pos != std::string::npos) {
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 1);

			// Trim whitespace using a lambda function and ranges
			auto trim = [](const std::string& str) {
				auto start = std::ranges::find_if_not(str, isspace);
				auto end = std::ranges::find_if_not(str | std::views::reverse, isspace);
				return start != str.end() ? std::string(start, end.base()) : "";
			};

			parsedData[trim(key)] = trim(value);
		}
	}

	return parsedData;
}

std::vector<std::string> splitIntoLines(const std::string& aContent) {
	std::vector<std::string> lines;
	auto split_view = std::views::split(aContent, '\n');

	for (auto&& part : split_view) {
		lines.emplace_back(std::string(part.begin(), part.end()));
	}

	return lines;
}

std::string extractIncludeName(const std::string &aLine) {
	std::size_t firstQuote = aLine.find('"');
	std::size_t lastQuote = aLine.rfind('"');
	if (firstQuote != std::string::npos
		&& lastQuote != std::string::npos
		&& firstQuote != lastQuote)
	{
		std::string includeName = aLine.substr(firstQuote + 1, lastQuote - firstQuote - 1);
		return includeName;
	}
	return "";
}

std::string processIncludesRecursive(
		int aSourceIndex,
		int &aNextFileIndex,
		const std::string &aContent,
		const ShaderFiles &aIncludeFiles,
		std::map<std::string, int> &aIncludeIndices)
{
	auto lines = splitIntoLines(aContent);

	std::stringstream buffer;
	int lineNumber = 1;
	if (aSourceIndex != 0) {
		buffer << "#line 1 " << aSourceIndex << "\n";
	}
	for ( auto & line: lines) {
		if (line.find("#include") == 0) {
			std::string includeName = extractIncludeName(line);
			if (includeName.empty()) {
				throw OpenGLError("Incorrectly formated shader include directive.");
			}
			auto pathIt = aIncludeFiles.find(includeName);
			if (pathIt == aIncludeFiles.end()) {
				throw OpenGLError("Unknown shader include: " + includeName);
			}
			auto indexIt = aIncludeIndices.find(includeName);
			int sourceIndex = -1;
			if (indexIt == aIncludeIndices.end()) {
				sourceIndex = aNextFileIndex++;
				aIncludeIndices[includeName] = sourceIndex;
			} else {
				sourceIndex = indexIt->second;
			}
			auto content = loadShaderSource(pathIt->second);
			buffer << processIncludesRecursive(sourceIndex, aNextFileIndex, content, aIncludeFiles, aIncludeIndices);  // Recursively process includes
			buffer << "#line " << ++lineNumber << " " << aSourceIndex << "\n";  // Reset the line number post-include
		} else {
			buffer << line << "\n";
			++lineNumber;
		}
	}

	return buffer.str();
}

std::string processIncludes(
		const std::string &aContent,
		const ShaderFiles &aIncludeFiles)
{
	int nextFileIndex = 1;
	std::map<std::string, int> includeIndices;
	auto newContent = processIncludesRecursive(0, nextFileIndex, aContent, aIncludeFiles, includeIndices);

	for (auto &source: includeIndices) {
		std::cout << "\t" << source.first << " - " << source.second << "\n";
	}
	return newContent;
}


const static std::map<std::string, GLenum> cShaderTypeEnums = {
	{ "vertex", GL_VERTEX_SHADER },
	{ "fragment", GL_FRAGMENT_SHADER },
	{ "geometry", GL_GEOMETRY_SHADER },
	{ "control", GL_TESS_CONTROL_SHADER },
	{ "evaluation", GL_TESS_EVALUATION_SHADER },
	{ "compute", GL_COMPUTE_SHADER },
};

void OGLMaterialFactory::loadShadersFromDir(fs::path aShaderDir) {
	aShaderDir = fs::canonical(aShaderDir);
	ShaderProgramFiles shaderFiles = listShaderFiles(aShaderDir);

	const auto &includeFiles = shaderFiles["include"];
	using CompiledShaderMap = std::map<std::string, OpenGLResource>;
	std::map<std::string, CompiledShaderMap> compiledShaders;
	for (auto & [shaderType, enumValue] : cShaderTypeEnums) {
		auto files = shaderFiles[shaderType];
		std::map<std::string, OpenGLResource> shaders;
		for (auto &shaderFile : files) {
			std::cout << "Compiling " << shaderType << " shader: " << shaderFile.second << "\n";
			auto content = loadShaderSource(shaderFile.second);
			content = processIncludes(content, includeFiles);
			auto compiledShader = compileShader(enumValue, content);
			shaders.emplace(shaderFile.first, std::move(compiledShader));
		}
		compiledShaders[shaderType] = std::move(shaders);
	}

	auto &programFiles = shaderFiles["program"];
	for (auto &programFile : programFiles) {
		std::cout << "Creating shader program: " << programFile.first << "\n";
		auto shaderNames = parseProgramFile(programFile.second);

		CompiledShaderStages shaderStages;
		for (auto &[shaderType, shaderName] : shaderNames) {
			auto &compShaders = compiledShaders[shaderType];
			auto it = compShaders.find(shaderName);
			if (it == compShaders.end()) {
				throw OpenGLError(
						"Program " + programFile.first + " cannot be linked. Shader ("
						+ shaderType + ") : " + shaderName + " was not compiled.");
			}
			shaderStages.push_back(&(it->second));
		}
		auto program = createShaderProgram(shaderStages);
		auto uniforms = listShaderUniforms(program);
		for (auto info : uniforms) {
			std::cout
				<< "Uniform name: " << info.name
				<< " Type: " << getGLTypeName(info.type)
				<< " Location: " << info.location << "\n";
		}
		mPrograms.emplace(
				programFile.first,
				std::make_shared<OGLShaderProgram>(
					std::move(program),
					std::move(uniforms)
					));
	}
	auto &computeShaders = compiledShaders["compute"];
	for (auto &shader : computeShaders) {
		std::cout << "Creating shader program: " << shader.first << "\n";
		auto program = createShaderProgram(CompiledShaderStages{ &(shader.second) });
		auto uniforms = listShaderUniforms(program);
		for (auto info : uniforms) {
			std::cout
				<< "Uniform name: " << info.name
				<< " Type: " << getGLTypeName(info.type)
				<< " Location: " << info.location << "\n";
		}
		mPrograms.emplace(
				shader.first,
				std::make_shared<OGLShaderProgram>(
					std::move(program),
					std::move(uniforms)
					));
	}

}

std::vector<fs::path> findImageFiles(const fs::path& aTextureDir) {
	if (!fs::exists(aTextureDir) || !fs::is_directory(aTextureDir)) {
		throw std::runtime_error("Texture dir path is not a directory or does not exist.");
	}
	std::cout << "Loading textures from directory: " << aTextureDir << "\n";
	std::vector<fs::path> imageFiles;

	for (const auto& entry : fs::recursive_directory_iterator(aTextureDir)) {
		if (entry.is_regular_file()) {
			auto ext = entry.path().extension().string();
			std::transform(ext.begin(), ext.end(), ext.begin(),
					[](unsigned char c){ return std::tolower(c); });

			if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".bmp" || ext == ".tga") {
				imageFiles.push_back(entry.path());
			}
		}
	}

	return imageFiles;
}

std::unique_ptr<ImageData> loadImage(const fs::path& filePath) {
	int width, height, channels;
	unsigned char* data = stbi_load(filePath.string().c_str(), &width, &height, &channels, 0);
	if (!data) {
		throw OpenGLError("Failed to load image: " + filePath.string());
	}

	return std::make_unique<ImageData>(data, width, height, channels);
}

OpenGLResource createTextureFromData(const ImageData& imgData) {
	auto textureID = createTexture();
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, textureID.get()));

	// Set texture wrapping and filtering options
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	GLenum format = GL_RGB;
	switch (imgData.channels) {
	case 1: format = GL_RED; break;
	case 2: format = GL_RG; break;
	case 3: format = GL_RGB; break;
	case 4: format = GL_RGBA; break;
	default:
		throw OpenGLError("Unsupported number of texture channels");
	};

	GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, format, imgData.width, imgData.height, 0, format, GL_UNSIGNED_BYTE, imgData.data.get()));
	GL_CHECK(glGenerateMipmap(GL_TEXTURE_2D));
	GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

	return textureID;
}


void OGLMaterialFactory::loadTexturesFromDir(fs::path aTextureDir) {
	aTextureDir = fs::canonical(aTextureDir);
	auto imageFiles = findImageFiles(aTextureDir);

	for (const auto& textureFile : imageFiles) {
		auto imageData = loadImage(textureFile);
		if (imageData) {
			auto texture = createTextureFromData(*imageData);

			auto name = convertToIdentifier(fs::relative(textureFile, aTextureDir).string());
			mTextures[name] = std::make_shared<OGLTexture>(std::move(texture));
			std::cout << "Loaded texture: " << name << " from " << textureFile << "\n";
		}
	}
}

std::vector<fs::path> findVolumeDataFiles(const fs::path& aTextureDir) {
	if (!fs::exists(aTextureDir) || !fs::is_directory(aTextureDir)) {
		throw std::runtime_error("3D Texture dir path is not a directory or does not exist.");
	}
	std::cout << "Loading 3D textures from directory: " << aTextureDir << "\n";
	std::vector<fs::path> imageFiles;

	for (const auto& entry : fs::recursive_directory_iterator(aTextureDir)) {
		if (entry.is_regular_file()) {
			auto ext = entry.path().extension().string();
			std::transform(ext.begin(), ext.end(), ext.begin(),
					[](unsigned char c){ return std::tolower(c); });

			if (ext == ".mhd" || ext == ".dump" ) {
				imageFiles.push_back(entry.path());
			}
		}
	}

	return imageFiles;
}

using DataBuffer = std::variant<
			std::vector<uint16_t>,
			std::vector<float>
			>;
struct VolumeData {
	DataBuffer data;
	int width, height, depth;
};

struct DimInfo {
	int32_t minimum = 0;
	int32_t maximum = 0;
	float elementExtent = 1.0;
};

template<typename TContainer>
void printMinMax(const TContainer &aContainer) {
	auto minIt = std::min_element(aContainer.begin(), aContainer.end());
	auto maxIt = std::max_element(aContainer.begin(), aContainer.end());

	if (minIt != aContainer.end() && maxIt != aContainer.end()) {
		std::cout << "Minimum value: " << *minIt << std::endl;
		std::cout << "Maximum value: " << *maxIt << std::endl;
	} else {
		std::cout << "The vector is empty." << std::endl;
	}
}

std::unique_ptr<VolumeData> loadDumpFile(const fs::path& aFilePath) {
	std::ifstream rawFile(aFilePath, std::ios::binary);
	uint8_t endianness;
	rawFile.read(reinterpret_cast<char*>(&endianness), 1);
	std::array<uint32_t, 3> header;
	rawFile.read(reinterpret_cast<char*>(header.data()), header.size() * sizeof(uint32_t));

        uint32_t dimension;
	rawFile.read(reinterpret_cast<char*>(&dimension), sizeof(uint32_t));
	// swapBytes(dimension);

        uint32_t elementTypeID;
	rawFile.read(reinterpret_cast<char*>(&elementTypeID), sizeof(uint32_t));
	// swapBytes(elementTypeID);


	std::array<DimInfo, 3> extents;
	rawFile.read(reinterpret_cast<char*>(extents.data()), extents.size() * sizeof(DimInfo));
        uint32_t headerEnd;
	rawFile.read(reinterpret_cast<char*>(&headerEnd), sizeof(uint32_t));

	std::cout << "Dimension " << dimension << " elementTypeID " << elementTypeID << "\n";
	for (int i = 0; i < 3; ++i) {
		std::cout << extents[i].minimum << " - " << extents[i].maximum << " - " << extents[i].elementExtent << "\n";
	}

	DataBuffer buffer;
	if (elementTypeID == 9) {
		buffer = std::vector<float>();
	} else if (elementTypeID == 4) {
		buffer = std::vector<uint16_t>();
	} else {
		throw OpenGLError("Unsupported element type for dump loader");
	}


	int width = extents[0].maximum - extents[0].minimum;
	int height = extents[1].maximum - extents[1].minimum;
	int depth = extents[2].maximum - extents[2].minimum;

	auto buffer2 = std::vector<float>();
	buffer2.resize(width * height * depth);
	std::visit([&](auto&& arg) {
			using T = std::decay_t<decltype(arg)>;
			using ValueType = typename T::value_type;
			arg.resize(width * height * depth);
			rawFile.read(reinterpret_cast<char*>(arg.data()), arg.size() * sizeof(ValueType));

			// std::copy(arg.begin(), arg.end(), buffer2.begin());
			// std::transform(arg.begin(), arg.end(), buffer2.begin(), [](auto &val) {return val;});
			// printMinMax(arg);

		}, buffer);

	return std::make_unique<VolumeData>(buffer, width, height, depth);
}

std::unique_ptr<VolumeData> loadMHDFile(const fs::path& aFilePath) {
	std::ifstream mhdFile(aFilePath);
	std::string line;
	fs::path rawFilePath;
	int width = 0, height = 0, depth = 0;

	DataBuffer buffer;
	// Parse MHD file
	while (std::getline(mhdFile, line)) {
		std::istringstream iss(line);
		std::string tag, equals;
		iss >> tag;
		if (tag == "DimSize") {
			iss >> equals >> width >> height >> depth;
		} else if (tag == "ElementDataFile") {
			std::string rawFilename;
			iss >> equals >> rawFilename;
			// std::getline(iss >> std::ws, rawFilename); // Read the rest of the line
			// Assuming RAW file is in the same directory as the MHD file
			rawFilePath = aFilePath.parent_path() / rawFilename;
		} else if (tag == "ElementType") {
			std::string elementType;
			iss >> equals >> elementType;
			if (elementType == "MET_FLOAT") {
				buffer = std::vector<float>();
			} else if (elementType == "MET_USHORT") {
				buffer = std::vector<uint16_t>();
			} else {
				throw OpenGLError("Unsupported element type for MHD loader");
			}
		}
	}

	// Load RAW file
	std::ifstream rawFile(rawFilePath, std::ios::binary);
	if (!rawFile.is_open()) {
		throw std::runtime_error("MHD loader failed to open raw file: " + rawFilePath.string());
	}

	std::visit([&](auto&& arg) {
			using T = std::decay_t<decltype(arg)>;
			using ValueType = typename T::value_type;
			arg.resize(width * height * depth);
			rawFile.read(reinterpret_cast<char*>(arg.data()), arg.size() * sizeof(ValueType));

			auto minIt = std::min_element(arg.begin(), arg.end());
			auto maxIt = std::max_element(arg.begin(), arg.end());

			if (minIt != arg.end() && maxIt != arg.end()) { // Check to ensure arg is not empty
				std::cout << "Minimum value: " << *minIt << std::endl;
				std::cout << "Maximum value: " << *maxIt << std::endl;
			} else {
				std::cout << "The vector is empty." << std::endl;
			}

		}, buffer);

	return std::make_unique<VolumeData>(buffer, width, height, depth);
}

std::unique_ptr<VolumeData> load3DFile(const fs::path& aFilePath) {
	auto ext = aFilePath.extension().string();
	std::transform(ext.begin(), ext.end(), ext.begin(),
			[](unsigned char c){ return std::tolower(c); });

	if (ext == ".mhd") {
		return loadMHDFile(aFilePath);
	}
	if (ext == ".dump" ) {
		return loadDumpFile(aFilePath);
	}

	throw OpenGLError("Unsupported 3d image type " + aFilePath.string());
}


struct TextureDataUploadVisitor {
	void operator()(const std::vector<uint16_t>& vec) const {
		GL_CHECK(glTexImage3D(GL_TEXTURE_3D, 0, GL_R16, volumeData.width, volumeData.height, volumeData.depth, 0, GL_RED, GL_UNSIGNED_SHORT, vec.data()));
	}

	void operator()(const std::vector<float>& vec) const {
		GL_CHECK(glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, volumeData.width, volumeData.height, volumeData.depth, 0, GL_RED, GL_FLOAT, vec.data()));
	}
	const VolumeData& volumeData;
};

OpenGLResource create3DTextureFromData(const VolumeData& volumeData) {
	auto texture = createTexture();
	glBindTexture(GL_TEXTURE_3D, texture.get());
	std::visit(TextureDataUploadVisitor{volumeData}, volumeData.data);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

	return texture;
}

void OGLMaterialFactory::load3DTexturesFromDir(fs::path aTextureDir) {
	aTextureDir = fs::canonical(aTextureDir);
	auto files = findVolumeDataFiles(aTextureDir);

	for (const auto& textureFile : files) {
		auto imageData = load3DFile(textureFile);
		if (imageData) {
			auto texture = create3DTextureFromData(*imageData);

			auto name = convertToIdentifier(fs::relative(textureFile, aTextureDir).string());
			mTextures[name] = std::make_shared<OGLTexture>(std::move(texture), GL_TEXTURE_3D);
			std::cout << "Loaded texture: " << name << " from " << textureFile << "\n";
		}
	}
}

ImageData::ImageData(unsigned char* data, int width, int height, int channels)
		: data(data, stbi_image_free), width(width), height(height), channels(channels) {}
