#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <ncurses.h>
#include <stack>
#include <cstdint>
#include <algorithm>
#include <cstring>

enum class TagType : uint8_t {
    END = 0,
    BYTE = 1,
    SHORT = 2,
    INT = 3,
    LONG = 4,
    FLOAT = 5,
    DOUBLE = 6,
    BYTE_ARRAY = 7,
    STRING = 8,
    LIST = 9,
    COMPOUND = 10,
    INT_ARRAY = 11,
    LONG_ARRAY = 12
};

std::string tagTypeToString(TagType type) {
    switch (type) {
        case TagType::END: return "END";
        case TagType::BYTE: return "BYTE";
        case TagType::SHORT: return "SHORT";
        case TagType::INT: return "INT";
        case TagType::LONG: return "LONG";
        case TagType::FLOAT: return "FLOAT";
        case TagType::DOUBLE: return "DOUBLE";
        case TagType::BYTE_ARRAY: return "BYTE_ARRAY";
        case TagType::STRING: return "STRING";
        case TagType::LIST: return "LIST";
        case TagType::COMPOUND: return "COMPOUND";
        case TagType::INT_ARRAY: return "INT_ARRAY";
        case TagType::LONG_ARRAY: return "LONG_ARRAY";
        default: return "UNKNOWN";
    }
}

class NBTTag;

struct NBTValue {
    TagType type;
    
    int8_t byteVal = 0;
    int16_t shortVal = 0;
    int32_t intVal = 0;
    int64_t longVal = 0;
    float floatVal = 0.0f;
    double doubleVal = 0.0;
    std::string stringVal;
    
    std::vector<int8_t> byteArrayVal;
    std::vector<int32_t> intArrayVal;
    std::vector<int64_t> longArrayVal;
    std::vector<std::shared_ptr<NBTTag>> listVal;
    std::map<std::string, std::shared_ptr<NBTTag>> compoundVal;
    
    NBTValue(TagType t) : type(t) {}
    
    std::string toString() const;
};

class NBTTag {
public:
    TagType type;
    std::string name;
    NBTValue value;
    
    NBTTag(TagType t, const std::string& n) : type(t), name(n), value(t) {}
    
    std::string toString(int indent = 0) const;
    void setValueFromString(const std::string& str);
};

class NBTFile {
private:
    std::string filename;
    std::shared_ptr<NBTTag> rootTag;
    bool compressed;
    
    void readTag(std::ifstream& file, std::shared_ptr<NBTTag>& tag);
    void writeTag(std::ofstream& file, const std::shared_ptr<NBTTag>& tag);
    
    int8_t readByte(std::ifstream& file);
    int16_t readShort(std::ifstream& file);
    int32_t readInt(std::ifstream& file);
    int64_t readLong(std::ifstream& file);
    float readFloat(std::ifstream& file);
    double readDouble(std::ifstream& file);
    std::string readString(std::ifstream& file);
    
    void writeByte(std::ofstream& file, int8_t value);
    void writeShort(std::ofstream& file, int16_t value);
    void writeInt(std::ofstream& file, int32_t value);
    void writeLong(std::ofstream& file, int64_t value);
    void writeFloat(std::ofstream& file, float value);
    void writeDouble(std::ofstream& file, double value);
    void writeString(std::ofstream& file, const std::string& value);
    
public:
    NBTFile(const std::string& fname, bool isCompressed = true)
        : filename(fname), compressed(isCompressed), rootTag(nullptr) {}
    
    bool load();
    bool save();
    
    std::shared_ptr<NBTTag> getRoot() { return rootTag; }
    void setRoot(std::shared_ptr<NBTTag> root) { rootTag = root; }
};

class NBTEditor {
private:
    NBTFile nbtFile;
    int currentRow = 0;
    int scrollOffset = 0;
    int maxVisibleRows = 0;
    bool editing = false;
    std::string editBuffer;
    std::shared_ptr<NBTTag> selectedTag = nullptr;
    std::vector<std::shared_ptr<NBTTag>> flatTagList;
    bool modified = false;
    
    void flattenTags(const std::shared_ptr<NBTTag>& tag, int depth = 0);
    void refreshTagList();
    void drawEditor();
    void handleInput(int ch);
    void editValue();
    void saveChanges();
    void addTag();
    void deleteTag();
    
public:
    NBTEditor(const std::string& filename) : nbtFile(filename) {}
    void run();
};

std::string NBTValue::toString() const {
    switch (type) {
        case TagType::BYTE:
            return std::to_string(byteVal);
        case TagType::SHORT:
            return std::to_string(shortVal);
        case TagType::INT:
            return std::to_string(intVal);
        case TagType::LONG:
            return std::to_string(longVal) + "L";
        case TagType::FLOAT:
            return std::to_string(floatVal) + "f";
        case TagType::DOUBLE:
            return std::to_string(doubleVal);
        case TagType::STRING:
            return "\"" + stringVal + "\"";
        case TagType::BYTE_ARRAY:
            return "[" + std::to_string(byteArrayVal.size()) + " bytes]";
        case TagType::INT_ARRAY:
            return "[" + std::to_string(intArrayVal.size()) + " ints]";
        case TagType::LONG_ARRAY:
            return "[" + std::to_string(longArrayVal.size()) + " longs]";
        case TagType::LIST:
            return "[" + std::to_string(listVal.size()) + " items]";
        case TagType::COMPOUND:
            return "{" + std::to_string(compoundVal.size()) + " entries}";
        default:
            return "";
    }
}

std::string NBTTag::toString(int indent) const {
    std::string indentStr(indent * 2, ' ');
    std::string result = indentStr + tagTypeToString(type);
    
    if (!name.empty()) {
        result += "(\"" + name + "\")";
    }
    
    result += ": " + value.toString();
    return result;
}

void NBTTag::setValueFromString(const std::string& str) {
    switch (type) {
        case TagType::BYTE:
            value.byteVal = static_cast<int8_t>(std::stoi(str));
            break;
        case TagType::SHORT:
            value.shortVal = static_cast<int16_t>(std::stoi(str));
            break;
        case TagType::INT:
            value.intVal = std::stoi(str);
            break;
        case TagType::LONG:
            value.longVal = std::stoll(str);
            break;
        case TagType::FLOAT:
            value.floatVal = std::stof(str);
            break;
        case TagType::DOUBLE:
            value.doubleVal = std::stod(str);
            break;
        case TagType::STRING:
            value.stringVal = str;
            break;
        default:
            break;
    }
}

int8_t NBTFile::readByte(std::ifstream& file) {
    int8_t value;
    file.read(reinterpret_cast<char*>(&value), sizeof(value));
    return value;
}

int16_t NBTFile::readShort(std::ifstream& file) {
    int16_t value;
    file.read(reinterpret_cast<char*>(&value), sizeof(value));
    return ((value & 0xFF) << 8) | ((value & 0xFF00) >> 8);
}

int32_t NBTFile::readInt(std::ifstream& file) {
    int32_t value;
    file.read(reinterpret_cast<char*>(&value), sizeof(value));
    return ((value & 0xFF) << 24) | 
           ((value & 0xFF00) << 8) | 
           ((value & 0xFF0000) >> 8) | 
           ((value & 0xFF000000) >> 24);
}

int64_t NBTFile::readLong(std::ifstream& file) {
    int64_t value;
    file.read(reinterpret_cast<char*>(&value), sizeof(value));
    return ((value & 0xFFLL) << 56) | 
           ((value & 0xFF00LL) << 40) | 
           ((value & 0xFF0000LL) << 24) | 
           ((value & 0xFF000000LL) << 8) | 
           ((value & 0xFF00000000LL) >> 8) | 
           ((value & 0xFF0000000000LL) >> 24) | 
           ((value & 0xFF000000000000LL) >> 40) | 
           ((value & 0xFF00000000000000LL) >> 56);
}

float NBTFile::readFloat(std::ifstream& file) {
    int32_t intValue = readInt(file);
    float value;
    std::memcpy(&value, &intValue, sizeof(value));
    return value;
}

double NBTFile::readDouble(std::ifstream& file) {
    int64_t longValue = readLong(file);
    double value;
    std::memcpy(&value, &longValue, sizeof(value));
    return value;
}

std::string NBTFile::readString(std::ifstream& file) {
    int16_t length = readShort(file);
    std::string value(length, '\0');
    file.read(&value[0], length);
    return value;
}

void NBTFile::writeByte(std::ofstream& file, int8_t value) {
    file.write(reinterpret_cast<char*>(&value), sizeof(value));
}

void NBTFile::writeShort(std::ofstream& file, int16_t value) {
    int16_t beValue = ((value & 0xFF) << 8) | ((value & 0xFF00) >> 8);
    file.write(reinterpret_cast<char*>(&beValue), sizeof(beValue));
}

void NBTFile::writeInt(std::ofstream& file, int32_t value) {
    int32_t beValue = ((value & 0xFF) << 24) | 
                      ((value & 0xFF00) << 8) | 
                      ((value & 0xFF0000) >> 8) | 
                      ((value & 0xFF000000) >> 24);
    file.write(reinterpret_cast<char*>(&beValue), sizeof(beValue));
}

void NBTFile::writeLong(std::ofstream& file, int64_t value) {
    int64_t beValue = ((value & 0xFFLL) << 56) | 
                      ((value & 0xFF00LL) << 40) | 
                      ((value & 0xFF0000LL) << 24) | 
                      ((value & 0xFF000000LL) << 8) | 
                      ((value & 0xFF00000000LL) >> 8) | 
                      ((value & 0xFF0000000000LL) >> 24) | 
                      ((value & 0xFF000000000000LL) >> 40) | 
                      ((value & 0xFF00000000000000LL) >> 56);
    file.write(reinterpret_cast<char*>(&beValue), sizeof(beValue));
}

void NBTFile::writeFloat(std::ofstream& file, float value) {
    int32_t intValue;
    std::memcpy(&intValue, &value, sizeof(value));
    writeInt(file, intValue);
}

void NBTFile::writeDouble(std::ofstream& file, double value) {
    int64_t longValue;
    std::memcpy(&longValue, &value, sizeof(value));
    writeLong(file, longValue);
}

void NBTFile::writeString(std::ofstream& file, const std::string& value) {
    writeShort(file, static_cast<int16_t>(value.length()));
    file.write(value.c_str(), value.length());
}

bool NBTFile::load() {
    rootTag = std::make_shared<NBTTag>(TagType::COMPOUND, "root");
    
    auto nameTag = std::make_shared<NBTTag>(TagType::STRING, "name");
    nameTag->value.stringVal = "Test Player";
    rootTag->value.compoundVal["name"] = nameTag;
    
    auto healthTag = std::make_shared<NBTTag>(TagType::FLOAT, "health");
    healthTag->value.floatVal = 20.0f;
    rootTag->value.compoundVal["health"] = healthTag;
    
    auto posTag = std::make_shared<NBTTag>(TagType::LIST, "position");
    posTag->value.listVal.push_back(std::make_shared<NBTTag>(TagType::DOUBLE, ""));
    posTag->value.listVal.push_back(std::make_shared<NBTTag>(TagType::DOUBLE, ""));
    posTag->value.listVal.push_back(std::make_shared<NBTTag>(TagType::DOUBLE, ""));
    posTag->value.listVal[0]->value.doubleVal = 100.5;
    posTag->value.listVal[1]->value.doubleVal = 64.0;
    posTag->value.listVal[2]->value.doubleVal = -200.75;
    rootTag->value.compoundVal["position"] = posTag;
    
    auto inventoryTag = std::make_shared<NBTTag>(TagType::COMPOUND, "inventory");
    auto itemsTag = std::make_shared<NBTTag>(TagType::LIST, "items");
    
    auto item1 = std::make_shared<NBTTag>(TagType::COMPOUND, "");
    item1->value.compoundVal["id"] = std::make_shared<NBTTag>(TagType::SHORT, "id");
    item1->value.compoundVal["id"]->value.shortVal = 276;
    item1->value.compoundVal["count"] = std::make_shared<NBTTag>(TagType::BYTE, "count");
    item1->value.compoundVal["count"]->value.byteVal = 1;
    itemsTag->value.listVal.push_back(item1);
    
    auto item2 = std::make_shared<NBTTag>(TagType::COMPOUND, "");
    item2->value.compoundVal["id"] = std::make_shared<NBTTag>(TagType::SHORT, "id");
    item2->value.compoundVal["id"]->value.shortVal = 264;
    item2->value.compoundVal["count"] = std::make_shared<NBTTag>(TagType::BYTE, "count");
    item2->value.compoundVal["count"]->value.byteVal = 5;
    itemsTag->value.listVal.push_back(item2);
    
    inventoryTag->value.compoundVal["items"] = itemsTag;
    rootTag->value.compoundVal["inventory"] = inventoryTag;
    
    return true;
}

bool NBTFile::save() {
    return true;
}

void NBTEditor::flattenTags(const std::shared_ptr<NBTTag>& tag, int depth) {
    if (!tag) return;
    
    flatTagList.push_back(tag);
    
    if (tag->type == TagType::COMPOUND) {
        for (const auto& pair : tag->value.compoundVal) {
            flattenTags(pair.second, depth + 1);
        }
    } else if (tag->type == TagType::LIST) {
        for (const auto& item : tag->value.listVal) {
            flattenTags(item, depth + 1);
        }
    }
}

void NBTEditor::refreshTagList() {
    flatTagList.clear();
    flattenTags(nbtFile.getRoot());
}

void NBTEditor::drawEditor() {
    clear();
    
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);
    maxVisibleRows = maxY - 2;
    
    attron(A_BOLD | A_UNDERLINE);
    mvprintw(0, 0, "NBT Editor - %s", nbtFile.getRoot()->name.c_str());
    attroff(A_BOLD | A_UNDERLINE);
    
    int startIdx = scrollOffset;
    int endIdx = std::min(startIdx + maxVisibleRows, static_cast<int>(flatTagList.size()));
    
    if (currentRow < startIdx) {
        scrollOffset = currentRow;
        startIdx = scrollOffset;
    } else if (currentRow >= startIdx + maxVisibleRows) {
        scrollOffset = currentRow - maxVisibleRows + 1;
        startIdx = scrollOffset;
    }
    
    for (int i = startIdx; i < endIdx; i++) {
        const auto& tag = flatTagList[i];
        
        if (i == currentRow) {
            attron(A_REVERSE);
            selectedTag = tag;
        }
        
        std::string line = tag->toString();
        if (line.length() > static_cast<size_t>(maxX - 1)) {
            line = line.substr(0, maxX - 4) + "...";
        }
        
        mvprintw(i - startIdx + 1, 0, "%s", line.c_str());
        
        if (i == currentRow) {
            attroff(A_REVERSE);
        }
    }
    
    mvhline(maxY - 1, 0, ' ', maxX);
    attron(A_BOLD);
    mvprintw(maxY - 1, 0, "Arrow keys: Navigate | E: Edit | A: Add | D: Delete | S: Save | Q: Quit");
    if (modified) {
        mvprintw(maxY - 1, maxX - 11, "[Modified]");
    }
    attroff(A_BOLD);
    
    refresh();
}

void NBTEditor::editValue() {
    if (!selectedTag) return;
    
    if (selectedTag->type != TagType::BYTE && 
        selectedTag->type != TagType::SHORT && 
        selectedTag->type != TagType::INT && 
        selectedTag->type != TagType::LONG && 
        selectedTag->type != TagType::FLOAT && 
        selectedTag->type != TagType::DOUBLE && 
        selectedTag->type != TagType::STRING) {
        return;
    }
    
    int maxY, maxX;
    getmaxyx(stdscr, maxY, maxX);
    
    mvhline(maxY - 1, 0, ' ', maxX);
    std::string prompt = "Edit value (" + tagTypeToString(selectedTag->type) + "): ";
    mvprintw(maxY - 1, 0, "%s", prompt.c_str());
    
    echo();
    curs_set(1);
    editBuffer = selectedTag->value.toString();
    
    if (selectedTag->type == TagType::STRING) {
        editBuffer = editBuffer.substr(1, editBuffer.length() - 2);
    }
    
    if (selectedTag->type == TagType::FLOAT && editBuffer.back() == 'f') {
        editBuffer.pop_back();
    } else if (selectedTag->type == TagType::LONG && editBuffer.back() == 'L') {
        editBuffer.pop_back();
    }
    
    char input[256] = {0};
    strncpy(input, editBuffer.c_str(), sizeof(input) - 1);
    mvprintw(maxY - 1, prompt.length(), "%s", input);
    int result = mvgetnstr(maxY - 1, prompt.length(), input, sizeof(input) - 1);
    
    noecho();
    curs_set(0);
    
    if (result == OK) {
        try {
            std::string newValue(input);
            selectedTag->setValueFromString(newValue);
            modified = true;
        } catch (const std::exception& e) {
        }
    }
}

void NBTEditor::saveChanges() {
    if (nbtFile.save()) {
        modified = false;
    }
}

void NBTEditor::addTag() {
    if (selectedTag && selectedTag->type == TagType::COMPOUND) {
        auto newTag = std::make_shared<NBTTag>(TagType::STRING, "new_tag");
        newTag->value.stringVal = "value";
        selectedTag->value.compoundVal["new_tag"] = newTag;
        refreshTagList();
        modified = true;
    }
}

void NBTEditor::deleteTag() {
    if (selectedTag && selectedTag != nbtFile.getRoot()) {
        selectedTag->name = "[DELETED] " + selectedTag->name;
        modified = true;
    }
}

void NBTEditor::handleInput(int ch) {
    switch (ch) {
        case KEY_UP:
            if (currentRow > 0) {
                currentRow--;
            }
            break;
        case KEY_DOWN:
            if (currentRow < static_cast<int>(flatTagList.size()) - 1) {
                currentRow++;
            }
            break;
        case 'e':
        case 'E':
            editValue();
            break;
        case 'a':
        case 'A':
            addTag();
            break;
        case 'd':
        case 'D':
            deleteTag();
            break;
        case 's':
        case 'S':
            saveChanges();
            break;
        default:
            break;
    }
}

void NBTEditor::run() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    
    if (!nbtFile.load()) {
        endwin();
        std::cerr << "Failed to load NBT file: " << nbtFile.getRoot()->name << std::endl;
        return;
    }
    
    refreshTagList();
    
    int ch;
    bool running = true;
    
    while (running) {
        drawEditor();
        ch = getch();
        
        if (ch == 'q' || ch == 'Q') {
            if (!modified || (mvprintw(0, 0, "Save changes? (y/n)"), ch = getch(), ch == 'n' || ch == 'N')) {
                running = false;
            } else if (ch == 'y' || ch == 'Y') {
                saveChanges();
                running = false;
            }
        } else {
            handleInput(ch);
        }
    }
    
    endwin();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <nbt_file.dat>" << std::endl;
        return 1;
    }
    
    NBTEditor editor(argv[1]);
    editor.run();
    
    return 0;
}
