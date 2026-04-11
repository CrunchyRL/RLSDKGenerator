#include "PiecesOfCode.hpp"

/*
#
=========================================================================================
# # Pieces Of Code #
=========================================================================================
#
*/

namespace PiecesOfCode {
    const std::string TArray_Iterator =
        "template<typename TArray>\n"
        "class TIterator\n"
        "{\n"
        "public:\n"
        "\tusing ElementType = typename TArray::ElementType;\n"
        "\tusing ElementPointer = ElementType*;\n"
        "\tusing ElementReference = ElementType&;\n"
        "\tusing ElementConstReference = const ElementType&;\n"
        "\n"
        "private:\n"
        "\tElementPointer IteratorData;\n"
        "\n"
        "public:\n"
        "\tTIterator(ElementPointer inElementPointer) : "
        "IteratorData(inElementPointer) {}\n"
        "\n"
        "\t~TIterator() {}\n"
        "\n"
        "public:\n"
        "\tTIterator& operator++()\n"
        "\t{\n"
        "\t\tIteratorData++;\n"
        "\t\treturn *this;\n"
        "\t}\n"
        "\n"
        "\tTIterator operator++(int32_t)\n"
        "\t{\n"
        "\t\tTIterator iteratorCopy = *this;\n"
        "\t\t++(*this);\n"
        "\t\treturn iteratorCopy;\n"
        "\t}\n"
        "\n"
        "\tTIterator& operator--()\n"
        "\t{\n"
        "\t\tIteratorData--;\n"
        "\t\treturn *this;\n"
        "\t}\n"
        "\n"
        "\tTIterator operator--(int32_t)\n"
        "\t{\n"
        "\t\tTIterator iteratorCopy = *this;\n"
        "\t\t--(*this);\n"
        "\t\treturn iteratorCopy;\n"
        "\t}\n"
        "\n"
        "\tElementReference operator[](int32_t index)\n"
        "\t{\n"
        "\t\treturn *(IteratorData[index]);\n"
        "\t}\n"
        "\n"
        "\tElementPointer operator->()\n"
        "\t{\n"
        "\t\treturn IteratorData;\n"
        "\t}\n"
        "\n"
        "\tElementReference operator*()\n"
        "\t{\n"
        "\t\treturn *IteratorData;\n"
        "\t}\n"
        "\n"
        "public:\n"
        "\tbool operator==(const TIterator& other) const\n"
        "\t{\n"
        "\t\treturn (IteratorData == other.IteratorData);\n"
        "\t}\n"
        "\n"
        "\tbool operator!=(const TIterator& other) const\n"
        "\t{\n"
        "\t\treturn !(*this == other);\n"
        "\t}\n"
        "};\n";

    const std::string TArray_Class = R"RLSDK(template<typename InElementType>
class TArray
{
public:
	using ElementType = InElementType;
	using ElementPointer = ElementType*;
	using ElementReference = ElementType&;
	using ElementConstPointer = const ElementType*;
	using ElementConstReference = const ElementType&;
	using Iterator = TIterator<TArray<ElementType>>;

private:
	ElementPointer ArrayData;
	int32_t ArrayCount;
	int32_t ArrayMax;

private:
	template<typename FunctionType>
	static FunctionType GetGMallocVirtualFunction(size_t index)
	{
		if (!GMalloc) {
			return nullptr;
		}

		void* instance = *reinterpret_cast<void**>(GMalloc);
		if (!instance) {
			return nullptr;
		}

		void** vTable = *reinterpret_cast<void***>(instance);
		return reinterpret_cast<FunctionType>(vTable[index]);
	}

	static void* GMallocAlloc(size_t bytes, uint32_t alignment = 8)
	{
		using FMallocType = void* (__fastcall*)(void*, uint32_t, uint32_t);
		if (auto func = GetGMallocVirtualFunction<FMallocType>(2)) {
			return func(GMalloc, static_cast<uint32_t>(bytes), alignment);
		}

		return nullptr;
	}

	static void* GMallocRealloc(void* original, size_t bytes, uint32_t alignment = 8)
	{
		using FReallocType = void* (__fastcall*)(void*, void*, uint32_t, uint32_t);
		if (auto func = GetGMallocVirtualFunction<FReallocType>(3)) {
			return func(GMalloc, original, static_cast<uint32_t>(bytes), alignment);
		}

		return nullptr;
	}

	static void GMallocFree(void* original)
	{
		if (!original) {
			return;
		}

		using FFreeType = void(__fastcall*)(void*, void*);
		if (auto func = GetGMallocVirtualFunction<FFreeType>(4)) {
			func(GMalloc, original);
		}
	}

	static int32_t CalculateGrowth(int32_t requestedCount)
	{
		if (requestedCount <= 0) {
			return 0;
		}

		return (requestedCount + 15) & ~15;
	}

public:
	TArray() : ArrayData(nullptr), ArrayCount(0), ArrayMax(0)
	{
		//ReAllocate(sizeof(ElementType));
	}

	~TArray()
	{
		//clear();
		//::operator delete(ArrayData, ArrayMax * sizeof(ElementType));
	}

public:
	ElementConstReference operator[](int32_t index) const
	{
		return ArrayData[index];
	}

	ElementReference operator[](int32_t index)
	{
		return ArrayData[index];
	}

	ElementConstReference at(int32_t index) const
	{
		return ArrayData[index];
	}

	ElementReference at(int32_t index)
	{
		return ArrayData[index];
	}

	ElementConstPointer data() const
	{
		return ArrayData;
	}

	void push_back(ElementConstReference newElement)
	{
		if (ArrayCount >= ArrayMax) {
			if (ArrayMax == 0) {
				Reserve(16);
			}
			else {
				Reserve(ArrayMax * 2);
			}
		}

		if (!ArrayData) {
			return;
		}

		if constexpr (std::is_trivially_copyable_v<ElementType>) {
			std::memcpy(ArrayData + ArrayCount, &newElement, sizeof(ElementType));
		}
		else {
			new(ArrayData + ArrayCount) ElementType(newElement);
		}

		++ArrayCount;
	}

	void push_back(ElementReference& newElement)
	{
		push_back(static_cast<ElementConstReference>(newElement));
	}

	int32_t AddItem(ElementConstReference newElement)
	{
		push_back(newElement);
		if (ArrayCount == 0) {
			return -1;
		}
		return ArrayCount - 1;
	}

	int32_t RemoveItem(ElementConstReference element)
	{
		for (int32_t index = 0; index < ArrayCount; ++index)
		{
			if (!(ArrayData[index] == element)) {
				continue;
			}

			Remove(index, 1);
			return index;
		}

		return -1;
	}

	void pop_back()
	{
		if (ArrayCount < 1) {
			return;
		}

		--ArrayCount;

		if constexpr (!std::is_trivially_copyable_v<ElementType>) {
			ArrayData[ArrayCount].~ElementType();
		}
	}

	void clear()
	{
		if constexpr (!std::is_trivially_copyable_v<ElementType>) {
			for (int32_t i = 0; i < ArrayCount; ++i) {
				ArrayData[i].~ElementType();
			}
		}

		ArrayCount = 0;
	}

	int32_t size() const
	{
		return ArrayCount;
	}

	int32_t capacity() const
	{
		return ArrayMax;
	}

	bool empty() const
	{
		if (ArrayData)
		{
			return (size() == 0);
		}

		return true;
	}

	Iterator begin()
	{
		return Iterator(ArrayData);
	}

	Iterator begin() const
	{
		return Iterator(ArrayData);
	}

	Iterator end()
	{
		return Iterator(ArrayData + ArrayCount);
	}

	Iterator end() const
	{
		return Iterator(ArrayData + ArrayCount);
	}

	void Reserve(int32_t capacity)
	{
		capacity = CalculateGrowth(capacity);
		if (capacity < ArrayMax) {
			return;
		}

		ElementPointer newArrayData = nullptr;
		if (ArrayData) {
			newArrayData = static_cast<ElementPointer>(GMallocRealloc(ArrayData, static_cast<size_t>(capacity) * sizeof(ElementType), alignof(ElementType)));
		}
		else {
			newArrayData = static_cast<ElementPointer>(GMallocAlloc(static_cast<size_t>(capacity) * sizeof(ElementType), alignof(ElementType)));
		}

		if (!newArrayData) {
			return;
		}

		ArrayData = newArrayData;
		ArrayMax = capacity;
	}

	void Remove(int32_t index, int32_t count = 1)
	{
		if (index < 0 || index >= ArrayCount || count <= 0) {
			return;
		}

		if (index + count > ArrayCount) {
			count = ArrayCount - index;
		}

		for (int32_t i = index; i < ArrayCount - count; ++i) {
			ArrayData[i] = std::move(ArrayData[i + count]);
		}

		if constexpr (!std::is_trivially_copyable_v<ElementType>) {
			for (int32_t i = ArrayCount - count; i < ArrayCount; ++i) {
				ArrayData[i].~ElementType();
			}
		}

		ArrayCount -= count;
	}

private:
	void ReAllocate(int32_t newArrayMax)
	{
		newArrayMax = (std::max)(newArrayMax, 0);

		if constexpr (!std::is_trivially_copyable_v<ElementType>) {
			if (newArrayMax < ArrayCount) {
				for (int32_t i = newArrayMax; i < ArrayCount; ++i) {
					ArrayData[i].~ElementType();
				}
			}
		}

		if (newArrayMax == 0) {
			if (ArrayData) {
				GMallocFree(ArrayData);
				ArrayData = nullptr;
			}
			ArrayCount = 0;
			ArrayMax = 0;
			return;
		}

		ElementPointer newArrayData = ArrayData
			? static_cast<ElementPointer>(GMallocRealloc(ArrayData, static_cast<size_t>(newArrayMax) * sizeof(ElementType), alignof(ElementType)))
			: static_cast<ElementPointer>(GMallocAlloc(static_cast<size_t>(newArrayMax) * sizeof(ElementType), alignof(ElementType)));

		if (!newArrayData) {
			return;
		}

		ArrayData = newArrayData;

		if constexpr (!std::is_trivially_copyable_v<ElementType>) {
			for (int32_t i = ArrayCount; i < newArrayMax; ++i) {
				new(ArrayData + i) ElementType();
			}
		}
		else if (newArrayMax > ArrayCount) {
			std::memset(ArrayData + ArrayCount, 0, static_cast<size_t>(newArrayMax - ArrayCount) * sizeof(ElementType));
		}

		ArrayCount = newArrayMax;
		ArrayMax = newArrayMax;
	}
};
)RLSDK";

    const std::string TMap_Class =
        "template<typename TKey, typename TValue>\n"
        "class TMap\n"
        "{\n"
        "private:\n"
        "\tstruct TPair\n"
        "\t{\n"
        "\t\tTKey Key;\n"
        "\t\tTValue Value;\n"
        "\t\tint32_t* HashNext;\n"
        "\t};\n"
        "\n"
        "public:\n"
        "\tusing ElementType = TPair;\n"
        "\tusing ElementPointer = ElementType*;\n"
        "\tusing ElementReference = ElementType&;\n"
        "\tusing ElementConstReference = const ElementType&;\n"
        "\tusing Iterator = TIterator<TArray<ElementType>>;\n"
        "\tusing ConstIterator = const ElementPointer;\n"
        "\n"
        "public:\n"
        "\tElementPointer Elements;\n"
        "\tint32_t ElementCount;\n"
        "\tint32_t ElementMax;\n"
        "\tuintptr_t IndirectData;\n"
        "\tint32_t InlineData[0x4];\n"
        "\tint32_t NumBits;\n"
        "\tint32_t MaxBits;\n"
        "\tint32_t FirstFreeIndex;\n"
        "\tint32_t NumFreeIndices;\n"
        "\tint64_t InlineHash;\n"
        "\tint32_t* Hash;\n"
        "\tint32_t HashCount;\n"
        "\n"
        "public:\n"
        "\tTMap() :\n"
        "\t\tElements(nullptr), ElementCount(0), ElementMax(0),\n"
        "\t\tIndirectData(NULL), NumBits(0), MaxBits(0),\n"
        "\t\tFirstFreeIndex(0), NumFreeIndices(0), InlineHash(0),\n"
        "\t\tHash(nullptr), HashCount(0)\n"
        "\t{}\n"
        "\n"
        "\tTMap(struct FMap_Mirror& other) : TMap()\n"
        "\t{\n"
        "\t\tassign(other);\n"
        "\t}\n"
        "\n"
        "\tTMap(const TMap<TKey, TValue>& other) : TMap()\n"
        "\t{\n"
        "\t\tassign(other);\n"
        "\t}\n"
        "\n"
        "\t~TMap() {}\n"
        "\n"
        "public:\n"
        "\tTMap<TKey, TValue>& assign(struct FMap_Mirror& other)\n"
        "\t{\n"
        "\t\t*this = *reinterpret_cast<TMap<TKey, TValue>*>(&other);\n"
        "\t\treturn *this;\n"
        "\t}\n"
        "\n"
        "\tTMap<TKey, TValue>& assign(const TMap<TKey, TValue>& other)\n"
        "\t{\n"
        "\t\tElements = other.Elements;\n"
        "\t\tElementCount = other.ElementCount;\n"
        "\t\tElementMax = other.ElementMax;\n"
        "\t\tIndirectData = other.IndirectData;\n"
        "\t\tInlineData[0] = other.InlineData[0];\n"
        "\t\tInlineData[1] = other.InlineData[1];\n"
        "\t\tInlineData[2] = other.InlineData[2];\n"
        "\t\tInlineData[3] = other.InlineData[3];\n"
        "\t\tNumBits = other.NumBits;\n"
        "\t\tMaxBits = other.MaxBits;\n"
        "\t\tFirstFreeIndex = other.FirstFreeIndex;\n"
        "\t\tNumFreeIndices = other.NumFreeIndices;\n"
        "\t\tInlineHash = other.InlineHash;\n"
        "\t\tHash = other.Hash;\n"
        "\t\tHashCount = other.HashCount;\n"
        "\t\treturn *this;\n"
        "\t}\n"
        "\n"
        "\tTValue& at(const TKey& key)\n"
        "\t{\n"
        "\t\tfor (int32_t i = 0; i < ElementCount; ++i)\n"
        "\t\t{\n"
        "\t\t\tif (Elements[i].Key == key) return Elements[i].Value;\n"
        "\t\t}\n"
        "\t\tstatic TValue DefaultValue; return DefaultValue;\n"
        "\t}\n"
        "\n"
        "\tconst TValue& at(const TKey& key) const\n"
        "\t{\n"
        "\t\tfor (int32_t i = 0; i < ElementCount; ++i)\n"
        "\t\t{\n"
        "\t\t\tif (Elements[i].Key == key) return Elements[i].Value;\n"
        "\t\t}\n"
        "\t\tstatic TValue DefaultValue; return DefaultValue;\n"
        "\t}\n"
        "\n"
        "\tTPair& at_index(int32_t index) { return Elements[index]; }\n"
        "\tconst TPair& at_index(int32_t index) const { return Elements[index]; }\n"
        "\n"
        "\tint32_t size() const { return ElementCount; }\n"
        "\tint32_t capacity() const { return ElementMax; }\n"
        "\tbool empty() const { return ElementCount == 0; }\n"
        "\n"
        "\tIterator begin() { return Elements; }\n"
        "\tIterator end() { return Elements + ElementCount; }\n"
        "\tConstIterator begin() const { return Elements; }\n"
        "\tConstIterator end() const { return Elements + ElementCount; }\n"
        "\n"
        "public:\n"
        "\tTValue& operator[](const TKey& key) { return at(key); }\n"
        "\tconst TValue& operator[](const TKey& key) const { return at(key); }\n"
        "\n"
        "\tTMap<TKey, TValue>& operator=(const struct FMap_Mirror& other)\n"
        "\t{\n"
        "\t\treturn assign(const_cast<struct FMap_Mirror&>(other));\n"
        "\t}\n"
        "\n"
        "\tTMap<TKey, TValue>& operator=(const TMap<TKey, TValue>& other)\n"
        "\t{\n"
        "\t\tif (this != &other) assign(other);\n"
        "\t\treturn *this;\n"
        "\t}\n"
        "};\n";

    const std::string FNameEntry_Struct = "class FNameEntry\n"
        "{\n"
        "public:";

    const std::string FNameEntry_Struct_UTF16 = R"RLSDK(
public:
	FNameEntry() : Flags(0), Index(-1), HashNext(nullptr), Name(L"None") {}
	~FNameEntry() {}

public:
	uint64_t GetFlags() const
	{
		return Flags;
	}

	int32_t GetIndex() const
	{
		return Index;
	}

	const wchar_t* GetWideName() const
	{
		return Name;
	}

	std::wstring ToWideString() const
	{
		const wchar_t* wideName = GetWideName();

		if (wideName)
		{
			return std::wstring(wideName);
		}

		return L"";
	}

	std::string ToString() const
	{
		return RLSDKDetail::WideToUtf8(GetWideName());
	}
};
)RLSDK";

    const std::string FNameEntry_Struct_UTF8 =
        "\npublic:\n"
        "\tFNameEntry() : Flags(0), Index(-1), HashNext(nullptr) {}\n"
        "\t~FNameEntry() {}\n"
        "\n"
        "public:\n"
        "\tuint64_t GetFlags() const\n"
        "\t{\n"
        "\t\treturn Flags;\n"
        "\t}\n"
        "\n"
        "\tint32_t GetIndex() const\n"
        "\t{\n"
        "\t\treturn Index;\n"
        "\t}\n"
        "\n"
        "\tconst char* GetAnsiName() const\n"
        "\t{\n"
        "\t\treturn Name;\n"
        "\t}\n"
        "\n"
        "\tstd::string ToString() const\n"
        "\t{\n"
        "\t\treturn std::string(Name);\n"
        "\t}\n"
        "};\n";

    const std::string FName_Struct_UTF16 = R"RLSDK(class FName
{
public:
	using ElementType = const wchar_t;
	using ElementPointer = ElementType*;

private:
	int32_t			FNameEntryId;									// 0x0000 (0x04)
	int32_t			InstanceNumber;									// 0x0004 (0x04)

private:
	static constexpr int32_t NAME_NO_NUMBER_INTERNAL = 0;

	static bool TryParseNumberedName(const wchar_t* rawName, std::wstring& outBaseName, int32_t& outInstanceNumber)
	{
		if (!rawName || !*rawName) {
			return false;
		}

		outBaseName = rawName;
		outInstanceNumber = NAME_NO_NUMBER_INTERNAL;

		const size_t length = outBaseName.length();
		if (length == 0) {
			return false;
		}

		const size_t underscoreIndex = outBaseName.find_last_of(L'_');
		if (underscoreIndex == std::wstring::npos || underscoreIndex + 1 >= length) {
			return false;
		}

		for (size_t i = underscoreIndex + 1; i < length; ++i) {
			const wchar_t ch = outBaseName[i];
			if (ch < L'0' || ch > L'9') {
				return false;
			}
		}

		outInstanceNumber = std::stoi(outBaseName.substr(underscoreIndex + 1)) + 1;
		outBaseName.resize(underscoreIndex);
		return true;
	}

	static int32_t FindEntryId(const wchar_t* nameToFind)
	{
		if (!nameToFind || !*nameToFind) {
			return -1;
		}

		static std::map<std::wstring, int32_t> foundNames{};
		const std::wstring cacheKey(nameToFind);

		if (const auto found = foundNames.find(cacheKey); found != foundNames.end()) {
			const int32_t cachedId = found->second;
			if (cachedId >= 0) {
				if (auto* names = Names()) {
					if (cachedId < names->size()) {
						if (auto* entry = names->at(cachedId)) {
							if (std::wcscmp(entry->GetWideName(), nameToFind) == 0) {
								return cachedId;
							}
						}
					}
				}
			}

			foundNames.erase(found);
		}

		auto* names = Names();
		if (!names) {
			return -1;
		}

		for (int32_t i = 0; i < names->size(); ++i)
		{
			auto* entry = names->at(i);
			if (!entry) {
				continue;
			}

			if (std::wcscmp(entry->GetWideName(), nameToFind) == 0)
			{
				foundNames.emplace(cacheKey, i);
				return i;
			}
		}

		return -1;
	}

	void AssignFromWideName(const wchar_t* rawName, int32_t instanceNumber = -1)
	{
		FNameEntryId = -1;
		InstanceNumber = NAME_NO_NUMBER_INTERNAL;

		if (!rawName || !*rawName) {
			return;
		}

		std::wstring baseName(rawName);
		int32_t resolvedInstanceNumber = NAME_NO_NUMBER_INTERNAL;

		if (instanceNumber == -1) {
			TryParseNumberedName(rawName, baseName, resolvedInstanceNumber);
		}
		else {
			resolvedInstanceNumber = instanceNumber;
		}

		const int32_t foundEntryId = FindEntryId(baseName.c_str());
		if (foundEntryId >= 0) {
			FNameEntryId = foundEntryId;
			InstanceNumber = resolvedInstanceNumber;
		}
	}

public:
	FName() : FNameEntryId(0), InstanceNumber(NAME_NO_NUMBER_INTERNAL) {}

	FName(int32_t id) : FNameEntryId(id), InstanceNumber(NAME_NO_NUMBER_INTERNAL) {}

	FName(const ElementPointer nameToFind) : FNameEntryId(-1), InstanceNumber(NAME_NO_NUMBER_INTERNAL)
	{
		AssignFromWideName(nameToFind);
	}

	FName(const FName& name) : FNameEntryId(name.FNameEntryId), InstanceNumber(name.InstanceNumber) {}
	FName(const char* nameToFind) : FNameEntryId(-1), InstanceNumber(NAME_NO_NUMBER_INTERNAL)
	{
		const std::wstring wideName = RLSDKDetail::Utf8ToWide(nameToFind);
		AssignFromWideName(wideName.c_str());
	}

	FName(const std::string& nameToFind) : FName(nameToFind.c_str()) {}

	~FName() {}

public:
	static class TArray<class FNameEntry*>* Names()
	{
		return reinterpret_cast<TArray<FNameEntry*>*>(GNames);
	}

	int32_t GetDisplayIndex() const
	{
		return FNameEntryId;
	}

	const FNameEntry GetDisplayNameEntry() const
	{
		if (IsValid())
		{
			return *Names()->at(FNameEntryId);
		}

		return FNameEntry();
	}

	FNameEntry* GetEntry()
	{
		if (!IsValid()) {
			return nullptr;
		}

		return Names()->at(FNameEntryId);
	}

	int32_t GetInstance() const
	{
		return InstanceNumber;
	}

	void SetInstance(int32_t newNumber)
	{
		InstanceNumber = newNumber;
	}

	std::string ToString() const
	{
		if (!IsValid()) {
			return "UnknownName";
		}

		std::string output = GetDisplayNameEntry().ToString();
		if (InstanceNumber > NAME_NO_NUMBER_INTERNAL) {
			output += "_" + std::to_string(InstanceNumber - 1);
		}

		return output;
	}

	std::wstring ToWideString() const
	{
		if (!IsValid()) {
			return L"UnknownName";
		}

		std::wstring output = GetDisplayNameEntry().ToWideString();
		if (InstanceNumber > NAME_NO_NUMBER_INTERNAL) {
			output += L"_" + std::to_wstring(InstanceNumber - 1);
		}

		return output;
	}

	bool IsValid() const
	{
		auto* names = Names();
		if (!names) {
			return false;
		}

		if (FNameEntryId < 0 || FNameEntryId >= names->size()) {
			return false;
		}

		return names->at(FNameEntryId) != nullptr;
	}

public:
	FName& operator=(const FName& other)
	{
		FNameEntryId = other.FNameEntryId;
		InstanceNumber = other.InstanceNumber;
		return *this;
	}

	bool operator==(const FName& other) const
	{
		uint64_t left = 0;
		uint64_t right = 0;
		std::memcpy(&left, this, sizeof(left));
		std::memcpy(&right, &other, sizeof(right));
		return left == right;
	}

	bool operator!=(const FName& other) const
	{
		return !(*this == other);
	}
};

static_assert(sizeof(FName) == 0x8, "FName must match UE3 x64 layout.");
)RLSDK";

    const std::string FName_Struct_UTF8 =
        "class FName\n"
        "{\n"
        "public:\n"
        "\tusing ElementType = const char;\n"
        "\tusing ElementPointer = ElementType*;\n"
        "\n"
        "private:\n"
        "\tint32_t\t\t\tFNameEntryId;\t\t\t\t\t\t\t\t\t// 0x0000 (0x04)\n"
        "\tint32_t\t\t\tInstanceNumber;\t\t\t\t\t\t\t\t\t// 0x0004 (0x04)\n"
        "\n"
        "public:\n"
        "\tFName() : FNameEntryId(-1), InstanceNumber(0) {}\n"
        "\n"
        "\tFName(int32_t id) : FNameEntryId(id), InstanceNumber(0) {}\n"
        "\n"
        "\tFName(ElementPointer nameToFind) : FNameEntryId(-1), InstanceNumber(0)\n"
        "\t{\n"
        "\t\tstatic std::vector<int32_t> nameCache{};\n"
        "\n"
        "\t\tfor (int32_t entryId : nameCache)\n"
        "\t\t{\n"
        "\t\t\tif (Names()->at(entryId))\n"
        "\t\t\t{\n"
        "\t\t\t\tif (strcmp(Names()->at(entryId)->Name, nameToFind) == 0)\n"
        "\t\t\t\t{\n"
        "\t\t\t\t\tFNameEntryId = entryId;\n"
        "\t\t\t\t\treturn;\n"
        "\t\t\t\t}\n"
        "\t\t\t}\n"
        "\t\t}\n"
        "\n"
        "\t\tfor (int32_t i = 0; i < Names()->size(); i++)\n"
        "\t\t{\n"
        "\t\t\tif (Names()->at(i))\n"
        "\t\t\t{\n"
        "\t\t\t\tif (strcmp(Names()->at(i)->Name, nameToFind) == 0)\n"
        "\t\t\t\t{\n"
        "\t\t\t\t\tnameCache.push_back(i);\n"
        "\t\t\t\t\tFNameEntryId = i;\n"
        "\t\t\t\t}\n"
        "\t\t\t}\n"
        "\t\t}\n"
        "\t}\n"
        "\n"
        "\tFName(const FName& name) : FNameEntryId(name.FNameEntryId), "
        "InstanceNumber(name.InstanceNumber) {}\n"
        "\n"
        "\t~FName() {}\n"
        "\n"
        "public:\n"
        "\tstatic TArray<FNameEntry*>* Names()\n"
        "\t{\n"
        "\t\tTArray<FNameEntry*>* recastedArray = "
        "reinterpret_cast<TArray<FNameEntry*>*>(GNames);\n"
        "\t\treturn recastedArray;\n"
        "\t}\n"
        "\n"
        "\tint32_t GetDisplayIndex() const\n"
        "\t{\n"
        "\t\treturn FNameEntryId;\n"
        "\t}\n"
        "\n"
        "\tconst FNameEntry GetDisplayNameEntry() const\n"
        "\t{\n"
        "\t\tif (IsValid())\n"
        "\t\t{\n"
        "\t\t\treturn *Names()->at(FNameEntryId);\n"
        "\t\t}\n"
        "\n"
        "\t\treturn FNameEntry();\n"
        "\t}\n"
        "\n"
        "\tFNameEntry* GetEntry()\n"
        "\t{\n"
        "\t\tif (IsValid())\n"
        "\t\t{\n"
        "\t\t\treturn Names()->at(FNameEntryId);\n"
        "\t\t}\n"
        "\n"
        "\t\treturn nullptr;\n"
        "\t}\n"
        "\n"
        "\tint32_t GetInstance() const\n"
        "\t{\n"
        "\t\treturn InstanceNumber;\n"
        "\t}\n"
        "\n"
        "\tvoid SetInstance(int32_t newNumber)\n"
        "\t{\n"
        "\t\tInstanceNumber = newNumber;\n"
        "\t}\n"
        "\n"
        "\tstd::string ToString() const\n"
        "\t{\n"
        "\t\tif (IsValid())\n"
        "\t\t{\n"
        "\t\t\treturn GetDisplayNameEntry().ToString();\n"
        "\t\t}\n"
        "\n"
        "\t\treturn \"UnknownName\";\n"
        "\t}\n"
        "\n"
        "\tbool IsValid() const\n"
        "\t{\n"
        "\t\tif ((FNameEntryId < 0 || FNameEntryId > Names()->size()))\n"
        "\t\t{\n"
        "\t\t\treturn false;\n"
        "\t\t}\n"
        "\n"
        "\t\treturn true;\n"
        "\t}\n"
        "\n"
        "public:\n"
        "\tFName& operator=(const FName& other)\n"
        "\t{\n"
        "\t\tFNameEntryId = other.FNameEntryId;\n"
        "\t\tInstanceNumber = other.InstanceNumber;\n"
        "\t\treturn *this;\n"
        "\t}\n"
        "\n"
        "\tbool operator==(const FName& other) const\n"
        "\t{\n"
        "\t\treturn ((FNameEntryId == other.FNameEntryId) && (InstanceNumber == "
        "other.InstanceNumber));\n"
        "\t}\n"
        "\n"
        "\tbool operator!=(const FName& other) const\n"
        "\t{\n"
        "\t\treturn !(*this == other);\n"
        "\t}\n"
        "};\n";

    const std::string FString_Class_UTF16 = R"RLSDK(class FString
{
public:
	using ElementType = wchar_t;
	using ElementPointer = ElementType*;
	using ElementConstPointer = const ElementType*;

private:
	ElementPointer	ArrayData;										// 0x0000 (0x08)
	int32_t			ArrayCount;										// 0x0008 (0x04)
	int32_t			ArrayMax;										// 0x000C (0x04)

private:
	static ElementPointer DuplicateWideBuffer(ElementConstPointer source, int32_t count)
	{
		if (!source || count <= 0) {
			return nullptr;
		}

		auto* destination = static_cast<ElementPointer>(RLSDKDetail::GMallocAlloc(static_cast<size_t>(count) * sizeof(ElementType), alignof(ElementType)));
		if (!destination) {
			return const_cast<ElementPointer>(source);
		}

		std::memcpy(destination, source, static_cast<size_t>(count) * sizeof(ElementType));
		return destination;
	}

	void AssignWide(ElementConstPointer source, int32_t explicitCount = -1)
	{
		if (!source || !*source) {
			ArrayData = nullptr;
			ArrayCount = 0;
			ArrayMax = 0;
			return;
		}

		const int32_t sourceCount = explicitCount >= 0
			? explicitCount
			: static_cast<int32_t>(std::wcslen(source) + 1);

		ArrayData = DuplicateWideBuffer(source, sourceCount);
		ArrayCount = ArrayData ? sourceCount : 0;
		ArrayMax = ArrayCount;
	}

public:
	FString() : ArrayData(nullptr), ArrayCount(0), ArrayMax(0) {}

	FString(ElementConstPointer other) : ArrayData(nullptr), ArrayCount(0), ArrayMax(0) { assign(other); }
	FString(const FString& other) : ArrayData(nullptr), ArrayCount(0), ArrayMax(0) { assign(other); }
	FString(const std::wstring& other) : ArrayData(nullptr), ArrayCount(0), ArrayMax(0) { AssignWide(other.c_str(), static_cast<int32_t>(other.length() + 1)); }
	FString(const char* other) : ArrayData(nullptr), ArrayCount(0), ArrayMax(0)
	{
		const std::wstring wide = RLSDKDetail::Utf8ToWide(other);
		AssignWide(wide.c_str(), static_cast<int32_t>(wide.length() + 1));
	}
	FString(const std::string& other) : FString(other.c_str()) {}

	~FString() {}

public:
	FString& assign(ElementConstPointer other)
	{
		AssignWide(other);
		return *this;
	}

	FString& assign(const FString& other)
	{
		AssignWide(other.ArrayData, other.ArrayCount);
		return *this;
	}

	std::wstring ToWideString() const
	{
		return empty() ? std::wstring() : std::wstring(c_str());
	}

	std::string ToString() const
	{
		return RLSDKDetail::WideToUtf8(c_str());
	}

	ElementConstPointer c_str() const
	{
		return ArrayData ? ArrayData : L"";
	}

	bool empty() const
	{
		return ArrayData == nullptr || ArrayCount == 0;
	}

	int32_t length() const
	{
		return ArrayCount;
	}

	int32_t size() const
	{
		return ArrayMax;
	}

	int32_t Len() const
	{
		return ArrayCount > 0 ? ArrayCount - 1 : 0;
	}

public:
	FString& operator=(ElementConstPointer other)
	{
		return assign(other);
	}

	FString& operator=(const FString& other)
	{
		return assign(other.c_str());
	}

	bool operator==(const FString& other) const
	{
		if (ArrayData == other.ArrayData && ArrayCount == other.ArrayCount && ArrayMax == other.ArrayMax) {
			return true;
		}

		if (ArrayCount != other.ArrayCount) {
			return false;
		}

		return std::wcscmp(c_str(), other.c_str()) == 0;
	}

	bool operator!=(const FString& other) const
	{
		return !(*this == other);
	}

	ElementConstPointer operator*() const
	{
		return c_str();
	}
};

static_assert(sizeof(FString) == 0x10, "FString must match UE3 x64 layout.");
)RLSDK";

    const std::string FString_Class_UTF8 =
        "class FString\n"
        "{\n"
        "public:\n"
        "\tusing ElementType = const char;\n"
        "\tusing ElementPointer = ElementType*;\n"
        "\n"
        "private:\n"
        "\tElementPointer\tArrayData;\t\t\t\t\t\t\t\t\t\t// 0x0000 (0x08)\n"
        "\tint32_t\t\t\tArrayCount;\t\t\t\t\t\t\t\t\t\t// 0x0008 (0x04)\n"
        "\tint32_t\t\t\tArrayMax;\t\t\t\t\t\t\t\t\t\t// 0x000C (0x04)\n"
        "\n"
        "public:\n"
        "\tFString() : ArrayData(nullptr), ArrayCount(0), ArrayMax(0) {}\n"
        "\n"
        "\tFString(ElementPointer other) : ArrayData(nullptr), ArrayCount(0), "
        "ArrayMax(0) { assign(other); }\n"
        "\n"
        "\t~FString() {}\n"
        "\n"
        "public:\n"
        "\tFString& assign(ElementPointer other)\n"
        "\t{\n"
        "\t\tArrayCount = (other ? (strlen(other) + 1) : 0);\n"
        "\t\tArrayMax = ArrayCount;\n"
        "\t\tArrayData = (ArrayCount > 0 ? other : nullptr);\n"
        "\t\treturn *this;\n"
        "\t}\n"
        "\n"
        "\tstd::string ToString() const\n"
        "\t{\n"
        "\t\tif (!empty())\n"
        "\t\t{\n"
        "\t\t\treturn std::string(c_str());\n"
        "\t\t}\n"
        "\n"
        "\t\treturn \"\";\n"
        "\t}\n"
        "\n"
        "\tElementPointer c_str() const\n"
        "\t{\n"
        "\t\treturn ArrayData;\n"
        "\t}\n"
        "\n"
        "\tbool empty() const\n"
        "\t{\n"
        "\t\tif (ArrayData)\n"
        "\t\t{\n"
        "\t\t\treturn (ArrayCount == 0);\n"
        "\t\t}\n"
        "\n"
        "\t\treturn true;\n"
        "\t}\n"
        "\n"
        "\tint32_t length() const\n"
        "\t{\n"
        "\t\treturn ArrayCount;\n"
        "\t}\n"
        "\n"
        "\tint32_t size() const\n"
        "\t{\n"
        "\t\treturn ArrayMax;\n"
        "\t}\n"
        "\n"
        "public:\n"
        "\tFString& operator=(ElementPointer other)\n"
        "\t{\n"
        "\t\treturn assign(other);\n"
        "\t}\n"
        "\n"
        "\tFString& operator=(const FString& other)\n"
        "\t{\n"
        "\t\treturn assign(other.c_str());\n"
        "\t}\n"
        "\n"
        "\tbool operator==(const FString& other)\n"
        "\t{\n"
        "\t\treturn (strcmp(ArrayData, other.ArrayData) == 0);\n"
        "\t}\n"
        "\n"
        "\tbool operator!=(const FString& other)\n"
        "\t{\n"
        "\t\treturn (strcmp(ArrayData, other.ArrayData) != 0);\n"
        "\t}\n"
        "};\n";

    const std::string FScriptDelegate_Struct =
        "struct FScriptDelegate\n"
        "{\n"
        "\tclass UObject* Object;\n"
        "\tclass FName FunctionName;\n"
        "\tuint64_t UniqueValue;\n"
        "\n"
        "\tbool operator==(const FScriptDelegate& other) const\n"
        "\t{\n"
        "\t\treturn Object == other.Object\n"
        "\t\t\t&& FunctionName == other.FunctionName\n"
        "\t\t\t&& UniqueValue == other.UniqueValue;\n"
        "\t}\n"
        "\n"
        "\tbool operator!=(const FScriptDelegate& other) const\n"
        "\t{\n"
        "\t\treturn !(*this == other);\n"
        "\t}\n"
        "};\n";

    const std::string FRepRecord_Struct = "struct FRepRecord\n"
        "{\n"
        "\tclass UProperty* Property;\n"
        "\tint32_t Index;\n"
        "};\n";

    const std::string FPointer_Struct =
        "struct FPointer\n"
        "{\n"
        "\tuintptr_t Dummy; // 0x0000 (0x04)\n"
        "\n"
        "\tbool operator==(const FPointer& other) const\n"
        "\t{\n"
        "\t\treturn Dummy == other.Dummy;\n"
        "\t}\n"
        "\n"
        "\tbool operator!=(const FPointer& other) const\n"
        "\t{\n"
        "\t\treturn !(*this == other);\n"
        "\t}\n"
        "};\n";

    const std::string FImplementedInterface_Struct =
        "struct FImplementedInterface\n"
        "{\n"
        "\tclass UClass* Class;\n"
        "\tclass UProperty* PointerProperty;\n"
        "};\n";

    const std::string FScriptInterface_Struct =
        "struct FScriptInterface\n"
        "{\n"
        "\tclass UObject* ObjectPointer;\n"
        "\tvoid* InterfacePointer;\n"
        "};\n";

    const std::string FQWord_Struct = "struct FQWord\n"
        "{\n"
        "\tint32_t A; // 0x0000 (0x04)\n"
        "\tint32_t B; // 0x0004 (0x04)\n"
        "};\n";

    const std::string FOutputDevice_Struct =
        "struct FOutputDevice\n"
        "{\n"
        "\tvoid* VfTable; // 0x0000 (0x04)\n"
        "\tunsigned long bAllowSuppression; // 0x0004 (0x04)\n"
        "\tunsigned long bSuppressEventTag; // 0x0008 (0x04)\n"
        "\tunsigned long bAutoEmitLineTerminator; // 0x000C (0x04)\n"
        "};\n";

    const std::string FOutParamRec_Struct =
        "struct FOutParamRec\n"
        "{\n"
        "\tclass UProperty* Property; // 0x0000 (0x04)\n"
        "\tunsigned char* PropertyAddr; // 0x0004 (0x04)\n"
        "\tstruct FOutParamRec* NextOutParam; // 0x0008 (0x04)\n"
        "};\n";

    const std::string FFrame_Struct =
        "struct FFrame : public FOutputDevice\n"
        "{\n"
        "\tclass UStruct* Node; // 0x0010 (0x04)\n"
        "\tclass UObject* Object; // 0x0014 (0x04)\n"
        "\tunsigned char* Code; // 0x0018 (0x04)\n"
        "\tunsigned char* Locals; // 0x001C (0x04)\n"
        "\tstruct FFrame* PreviousFrame; // 0x0020 (0x04)\n"
        "\tstruct FOutParamRec* OutParams; // 0x0024 (0x04)\n"
        "};\n";

    const std::string UClass_FunctionDescriptions =
        "\tbool IsChildOf(const class UStruct* SomeBase) const;\n\n";

    const std::string UObject_FunctionDescriptions =
        "\tstatic class TArray<class UObject*>* GObjObjects();\n"
        "\tstatic class UObject* GetDefaultInstance();\n"
        "\tstatic class UObject* GetDefaultInstance(class UClass* uClass);\n"
        "\n"
        "\tstd::string GetName();\n"
        "\tstd::string GetNameCPP();\n"
        "\tstd::string GetFullName();\n"
        "\tclass UObject* GetPackageObj();\n"
        "\ttemplate<typename T> static T* FindObject(const std::string& "
        "objectFullName)\n"
        "\t{\n"
        "\t\tfor (UObject* uObject : *UObject::GObjObjects())\n"
        "\t\t{\n"
        "\t\t\tif (uObject && uObject->IsA<T>())\n"
        "\t\t\t{\n"
        "\t\t\t\tif (uObject->GetFullName() == objectFullName)\n"
        "\t\t\t\t{\n"
        "\t\t\t\t\treturn reinterpret_cast<T*>(uObject);\n"
        "\t\t\t\t}\n"
        "\t\t\t}\n"
        "\t\t}\n"
        "\n"
        "\t\treturn nullptr;\n"
        "\t}\n"
        "\tstatic class UClass* FindClass(const std::string& classFullName);\n"
        "\tbool IsA(class UClass* uClass);\n"
        "\tbool IsA(int32_t objInternalInteger);\n"
        "\ttemplate<typename T> bool IsA()\n"
        "\t{\n"
        "\t\treturn IsA(T::StaticClass());\n"
        "\t}\n\n";

    const std::string UClass_Functions =
        "bool UClass::IsChildOf(const UStruct* SomeBase) const\n"
        "{\n"
        "\t__try {\n"
        "\t\tfor (const UStruct* Struct = this; Struct; Struct = (UStruct*)(Struct->SuperField->IsA<UClass>() ? Struct->SuperField : nullptr))\n"
        "\t\t{\n"
        "\t\t\tif (Struct == SomeBase)\n"
        "\t\t\t{\n"
        "\t\t\t\treturn true;\n"
        "\t\t\t}\n"
        "\t\t}\n"
        "\t}\n"
        "\t__except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION\n"
        "\t\t? EXCEPTION_EXECUTE_HANDLER\n"
        "\t\t: EXCEPTION_CONTINUE_SEARCH) {\n"
        "\t\treturn false;\n"
        "\t}\n"
        "\n"
        "\treturn false;\n"
        "}\n\n";

    const std::string UObject_Functions =
        "class TArray<class UObject*>* UObject::GObjObjects()\n"
        "{\n"
        "\treturn reinterpret_cast<TArray<UObject*>*>(GObjects);\n"
        "}\n"
        "\n"
        "class UObject* UObject::GetDefaultInstance()\n"
        "{\n"
        "\treturn GetDefaultInstance(StaticClass());\n"
        "}\n"
        "\n"
        "class UObject* UObject::GetDefaultInstance(class UClass* uClass)\n"
        "{\n"
        "\treturn (uClass ? uClass->ClassDefaultObject : nullptr);\n"
        "}\n"
        "\n"
        "std::string UObject::GetName()\n"
        "{\n"
        "\treturn this->Name.ToString();\n"
        "}\n"
        "\n"
        "std::string UObject::GetNameCPP()\n"
        "{\n"
        "\tstd::string nameCPP;\n"
        "\n"
        "\tif (this->IsA<UClass>())\n"
        "\t{\n"
        "\t\tUClass* uClass = reinterpret_cast<UClass*>(this);\n"
        "\n"
        "\t\twhile (uClass)\n"
        "\t\t{\n"
        "\t\t\tstd::string className = uClass->GetName();\n"
        "\n"
        "\t\t\tif (className == \"Actor\")\n"
        "\t\t\t{\n"
        "\t\t\t\tnameCPP += \"A\";\n"
        "\t\t\t\tbreak;\n"
        "\t\t\t}\n"
        "\t\t\telse if (className == \"Object\")\n"
        "\t\t\t{\n"
        "\t\t\t\tnameCPP += \"U\";\n"
        "\t\t\t\tbreak;\n"
        "\t\t\t}\n"
        "\n"
        "\t\t\tuClass = reinterpret_cast<UClass*>(uClass->SuperField);\n"
        "\t\t}\n"
        "\t}\n"
        "\telse\n"
        "\t{\n"
        "\t\tnameCPP += \"F\";\n"
        "\t}\n"
        "\n"
        "\tnameCPP += this->GetName();\n"
        "\n"
        "\treturn nameCPP;\n"
        "}\n"
        "\n"
        "std::string UObject::GetFullName()\n"
        "{\n"
        "\tstd::string fullName = this->GetName();\n"
        "\n"
        "\tfor (UObject* uOuter = this->Outer; uOuter; uOuter = uOuter->Outer)\n"
        "\t{\n"
        "\t\tfullName = (uOuter->GetName() + \".\" + fullName);\n"
        "\t}\n"
        "\n"
        "\tfullName = (this->Class->GetName() + \" \" + fullName);\n"
        "\treturn fullName;\n"
        "}\n"
        "\n"
        "class UObject* UObject::GetPackageObj()\n"
        "{\n"
        "\tUObject* uPackage = nullptr;\n"
        "\n"
        "\tfor (UObject* uOuter = this->Outer; uOuter; uOuter = uOuter->Outer)\n"
        "\t{\n"
        "\t\tuPackage = uOuter;\n"
        "\t}\n"
        "\n"
        "\treturn uPackage;\n"
        "}\n"
        "\n"
        "class UClass* UObject::FindClass(const std::string& classFullName)\n"
        "{\n"
        "\tstatic std::map<std::string, UClass*> classCache;\n"
        "\n"
        "\tif (classCache.empty())\n"
        "\t{\n"
        "\t\tfor (int32_t i = 0; i < (UObject::GObjObjects()->size() - 1); i++)\n"
        "\t\t{\n"
        "\t\t\tUObject* uObject = UObject::GObjObjects()->at(i);\n"
        "\n"
        "\t\t\tif (uObject)\n"
        "\t\t\t{\n"
        "\t\t\t\tstd::string objectFullName = uObject->GetFullName();\n"
        "\n"
        "\t\t\t\tif (objectFullName.find(\"Class\") == 0)\n"
        "\t\t\t\t{\n"
        "\t\t\t\t\tclassCache[objectFullName] = "
        "reinterpret_cast<UClass*>(uObject);\n"
        "\t\t\t\t}\n"
        "\t\t\t}\n"
        "\t\t}\n"
        "\t}\n"
        "\n"
        "\tif (classCache.contains(classFullName))\n"
        "\t{\n"
        "\t\treturn classCache[classFullName];\n"
        "\t}\n"
        "\n"
        "\treturn nullptr;\n"
        "}\n"
        "\n"
        "bool UObject::IsA(class UClass* uClass)\n"
        "{\n"
        "\t__try {\n"
        "\t\tfor (UClass* uSuperClass = reinterpret_cast<UClass*>(this->Class); uSuperClass; uSuperClass = reinterpret_cast<UClass*>(uSuperClass->SuperField))\n"
        "\t\t{\n"
        "\t\t\tif (uSuperClass == uClass)\n"
        "\t\t\t{\n"
        "\t\t\t\treturn true;\n"
        "\t\t\t}\n"
        "\t\t}\n"
        "\t}\n"
        "\t__except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION\n"
        "\t\t? EXCEPTION_EXECUTE_HANDLER\n"
        "\t\t: EXCEPTION_CONTINUE_SEARCH) {\n"
        "\t\treturn false;\n"
        "\t}\n"
        "\n"
        "\treturn false;\n"
        "}\n"
        "\n"
        "bool UObject::IsA(int32_t objInternalInteger)\n"
        "{\n"
        "\t__try {\n"
        "\t\tif (objInternalInteger > UObject::GObjObjects()->size() || objInternalInteger < 0) {\n"
        "\t\t\treturn false;\n"
        "\t\t}\n"
        "\n"
        "\t\tUClass* uClass = reinterpret_cast<UClass*>(UObject::GObjObjects()->at(objInternalInteger)->Class);\n"
        "\n"
        "\t\tif (uClass)\n"
        "\t\t{\n"
        "\t\t\treturn this->IsA(uClass);\n"
        "\t\t}\n"
        "\t}\n"
        "\t__except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION\n"
        "\t\t? EXCEPTION_EXECUTE_HANDLER\n"
        "\t\t: EXCEPTION_CONTINUE_SEARCH) {\n"
        "\t\treturn false;\n"
        "\t}\n"
        "\n"
        "\treturn false;\n"
        "}\n\n";

    const std::string UFunction_Functions =
        "class UFunction* UFunction::FindFunction(const std::string& "
        "functionFullName)\n"
        "{\n"
        "\tstatic std::map<std::string, UFunction*> functionCache;\n"
        "\n"
        "\tif (functionCache.empty())\n"
        "\t{\n"
        "\t\tfor (int32_t i = 0; i < (UObject::GObjObjects()->size() - 1); i++)\n"
        "\t\t{\n"
        "\t\t\tUObject* uObject = UObject::GObjObjects()->at(i);\n"
        "\n"
        "\t\t\tif (uObject)\n"
        "\t\t\t{\n"
        "\t\t\t\tstd::string objectFullName = uObject->GetFullName();\n"
        "\n"
        "\t\t\t\tif (objectFullName.find(\"Function\") == 0)\n"
        "\t\t\t\t{\n"
        "\t\t\t\t\tfunctionCache[objectFullName] = "
        "reinterpret_cast<UFunction*>(uObject);\n"
        "\t\t\t\t}\n"
        "\t\t\t}\n"
        "\t\t}\n"
        "\t}\n"
        "\n"
        "\tif (functionCache.contains(functionFullName))\n"
        "\t{\n"
        "\t\treturn functionCache[functionFullName];\n"
        "\t}\n"
        "\n"
        "\treturn nullptr;\n"
        "}\n\n";

    const std::string EEnumFlags =
        "// "
        "https://github.com/CodeRedModding/UnrealEngine3/blob/main/Development/Src/"
        "Core/Inc/UnStack.h#L48\n"
        "// State Flags\n"
        "enum EStateFlags\n"
        "{\n"
        "\tSTATE_Editable =\t\t\t\t\t\t0x00000001,\t// State should be "
        "user-selectable in UnrealEd.\n"
        "\tSTATE_Auto =\t\t\t\t\t\t\t0x00000002,\t// State is automatic (the "
        "default state).\n"
        "\tSTATE_Simulated =\t\t\t\t\t\t0x00000004, // State executes on client "
        "side.\n"
        "\tSTATE_HasLocals =\t\t\t\t\t\t0x00000008,\t// State has local "
        "variables.\n"
        "};\n"
        "\n"
        "// "
        "https://github.com/CodeRedModding/UnrealEngine3/blob/main/Development/Src/"
        "Core/Inc/UnStack.h#L60\n"
        "// Function Flags\n"
        "enum EFunctionFlags : uint64_t\n"
        "{\n"
        "\tFUNC_None =\t\t\t\t\t\t\t\t0x00000000,\n"
        "\tFUNC_Final =\t\t\t\t\t\t\t0x00000001,\n"
        "\tFUNC_Defined =\t\t\t\t\t\t\t0x00000002,\n"
        "\tFUNC_Iterator =\t\t\t\t\t\t\t0x00000004,\n"
        "\tFUNC_Latent =\t\t\t\t\t\t\t0x00000008,\n"
        "\tFUNC_PreOperator =\t\t\t\t\t\t0x00000010,\n"
        "\tFUNC_Singular =\t\t\t\t\t\t\t0x00000020,\n"
        "\tFUNC_Net =\t\t\t\t\t\t\t\t0x00000040,\n"
        "\tFUNC_NetReliable =\t\t\t\t\t\t0x00000080,\n"
        "\tFUNC_Simulated =\t\t\t\t\t\t0x00000100,\n"
        "\tFUNC_Exec =\t\t\t\t\t\t\t\t0x00000200,\n"
        "\tFUNC_Native =\t\t\t\t\t\t\t0x00000400,\n"
        "\tFUNC_Event =\t\t\t\t\t\t\t0x00000800,\n"
        "\tFUNC_Operator =\t\t\t\t\t\t\t0x00001000,\n"
        "\tFUNC_Static =\t\t\t\t\t\t\t0x00002000,\n"
        "\tFUNC_NoExport =\t\t\t\t\t\t\t0x00004000,\n"
        "\tFUNC_OptionalParm =\t\t\t\t\t\t0x00004000,\n"
        "\tFUNC_Const =\t\t\t\t\t\t\t0x00008000,\n"
        "\tFUNC_Invariant =\t\t\t\t\t\t0x00010000,\n"
        "\tFUNC_Public =\t\t\t\t\t\t\t0x00020000,\n"
        "\tFUNC_Private =\t\t\t\t\t\t\t0x00040000,\n"
        "\tFUNC_Protected =\t\t\t\t\t\t0x00080000,\n"
        "\tFUNC_Delegate =\t\t\t\t\t\t\t0x00100000,\n"
        "\tFUNC_NetServer =\t\t\t\t\t\t0x00200000,\n"
        "\tFUNC_HasOutParms =\t\t\t\t\t\t0x00400000,\n"
        "\tFUNC_HasDefaults =\t\t\t\t\t\t0x00800000,\n"
        "\tFUNC_NetClient =\t\t\t\t\t\t0x01000000,\n"
        "\tFUNC_DLLImport =\t\t\t\t\t\t0x02000000,\n"
        "\n"
        "\tFUNC_K2Call =\t\t\t\t\t\t\t0x04000000,\n"
        "\tFUNC_K2Override =\t\t\t\t\t\t0x08000000,\n"
        "\tFUNC_K2Pure =\t\t\t\t\t\t\t0x10000000,\n"
        "\tFUNC_EditorOnly =\t\t\t\t\t\t0x20000000,\n"
        "\tFUNC_Lambda =\t\t\t\t\t\t\t0x40000000,\n"
        "\tFUNC_NetValidate =\t\t\t\t\t\t0x80000000,\n"
        "\n"
        "\tFUNC_FuncInherit =\t\t\t\t\t\t(FUNC_Exec | FUNC_Event),\n"
        "\tFUNC_FuncOverrideMatch =\t\t\t\t(FUNC_Exec | FUNC_Final | FUNC_Latent | "
        "FUNC_PreOperator | FUNC_Iterator | FUNC_Static | FUNC_Public | "
        "FUNC_Protected | FUNC_Const),\n"
        "\tFUNC_NetFuncFlags =\t\t\t\t\t\t(FUNC_Net | FUNC_NetReliable | "
        "FUNC_NetServer | FUNC_NetClient),\n"
        "\n"
        "\tFUNC_AllFlags =\t\t\t\t\t\t\t0xFFFFFFFF\n"
        "};\n"
        "\n"
        "// "
        "https://github.com/CodeRedModding/UnrealEngine3/blob/main/Development/Src/"
        "Core/Inc/UnObjBas.h#L238\n"
        "// Proprerty Flags\n"
        "enum EPropertyFlags : uint64_t\n"
        "{\n"
        "\tCPF_Edit =\t\t\t\t\t\t\t\t0x0000000000000001,\t// Property is "
        "user-settable in the editor.\n"
        "\tCPF_Const =\t\t\t\t\t\t\t\t0x0000000000000002,\t// Actor\'s property "
        "always matches class\'s default actor property.\n"
        "\tCPF_Input =\t\t\t\t\t\t\t\t0x0000000000000004,\t// Variable is writable "
        "by the input system.\n"
        "\tCPF_ExportObject =\t\t\t\t\t\t0x0000000000000008,\t// Object can be "
        "exported with actor.\n"
        "\tCPF_OptionalParm =\t\t\t\t\t\t0x0000000000000010,\t// Optional "
        "parameter (if CPF_Param is set).\n"
        "\tCPF_Net =\t\t\t\t\t\t\t\t0x0000000000000020,\t// Property is relevant "
        "to network replication.\n"
        "\tCPF_EditFixedSize =\t\t\t\t\t\t0x0000000000000040, // Indicates that "
        "elements of an array can be modified, but its size cannot be changed.\n"
        "\tCPF_Parm =\t\t\t\t\t\t\t\t0x0000000000000080,\t// Function/When call "
        "parameter.\n"
        "\tCPF_OutParm =\t\t\t\t\t\t\t0x0000000000000100,\t// Value is copied out "
        "after function call.\n"
        "\tCPF_SkipParm =\t\t\t\t\t\t\t0x0000000000000200,\t// Property is a "
        "short-circuitable evaluation function parm.\n"
        "\tCPF_ReturnParm =\t\t\t\t\t\t0x0000000000000400,\t// Return value.\n"
        "\tCPF_CoerceParm =\t\t\t\t\t\t0x0000000000000800,\t// Coerce args into "
        "this function parameter.\n"
        "\tCPF_Native =\t\t\t\t\t\t\t0x0000000000001000,\t// Property is native: "
        "C++ code is responsible for serializing it.\n"
        "\tCPF_Transient =\t\t\t\t\t\t\t0x0000000000002000,\t// Property is "
        "transient: shouldn\'t be saved, zero-filled at load time.\n"
        "\tCPF_Config =\t\t\t\t\t\t\t0x0000000000004000,\t// Property should be "
        "loaded/saved as permanent profile.\n"
        "\tCPF_Localized =\t\t\t\t\t\t\t0x0000000000008000,\t// Property should be "
        "loaded as localizable text.\n"
        "\tCPF_Travel =\t\t\t\t\t\t\t0x0000000000010000,\t// Property travels "
        "across levels/servers.\n"
        "\tCPF_EditConst =\t\t\t\t\t\t\t0x0000000000020000,\t// Property is "
        "uneditable in the editor.\n"
        "\tCPF_GlobalConfig =\t\t\t\t\t\t0x0000000000040000,\t// Load config from "
        "base class, not subclass.\n"
        "\tCPF_Component =\t\t\t\t\t\t\t0x0000000000080000,\t// Property containts "
        "component references.\n"
        "\tCPF_AlwaysInit =\t\t\t\t\t\t0x0000000000100000,\t// Property should "
        "never be exported as NoInit(@todo - this doesn\'t need to be a property "
        "flag...only used during make).\n"
        "\tCPF_DuplicateTransient =\t\t\t\t0x0000000000200000, // Property should "
        "always be reset to the default value during any type of duplication "
        "(copy/paste, binary duplication, etc.).\n"
        "\tCPF_NeedCtorLink =\t\t\t\t\t\t0x0000000000400000,\t// Fields need "
        "construction/destruction.\n"
        "\tCPF_NoExport =\t\t\t\t\t\t\t0x0000000000800000,\t// Property should not "
        "be exported to the native class header file.\n"
        "\tCPF_NoClear =\t\t\t\t\t\t\t0x0000000002000000,\t// Hide clear (and "
        "browse) button.\n"
        "\tCPF_EditInline =\t\t\t\t\t\t0x0000000004000000,\t// Edit this object "
        "reference inline.\t\n"
        "\tCPF_EditInlineUse =\t\t\t\t\t\t0x0000000010000000,\t// EditInline with "
        "Use button.\n"
        "\tCPF_EditFindable =\t\t\t\t\t\t0x0000000008000000,\t// References are "
        "set by clicking on actors in the editor viewports.\n"
        "\tCPF_Deprecated =\t\t\t\t\t\t0x0000000020000000,\t// Property is "
        "deprecated.  Read it from an archive, but don\'t save it.\t\n"
        "\tCPF_DataBinding =\t\t\t\t\t\t0x0000000040000000,\t// Indicates that "
        "this property should be exposed to data stores.\n"
        "\tCPF_SerializeText =\t\t\t\t\t\t0x0000000080000000,\t// Native property "
        "should be serialized as text (ImportText, ExportText).\n"
        "\tCPF_RepNotify =\t\t\t\t\t\t\t0x0000000100000000,\t// Notify actors when "
        "a property is replicated.\n"
        "\tCPF_Interp =\t\t\t\t\t\t\t0x0000000200000000,\t// Interpolatable "
        "property for use with matinee.\n"
        "\tCPF_NonTransactional =\t\t\t\t\t0x0000000400000000,\t// Property isn\'t "
        "transacted.\n"
        "\tCPF_EditorOnly =\t\t\t\t\t\t0x0000000800000000,\t// Property should "
        "only be loaded in the editor.\n"
        "\tCPF_NotForConsole =\t\t\t\t\t\t0x0000001000000000, // Property should "
        "not be loaded on console (or be a console cooker commandlet).\n"
        "\tCPF_RepRetry =\t\t\t\t\t\t\t0x0000002000000000, // Property replication "
        "of this property if it fails to be fully sent (e.g. object references not "
        "yet available to serialize over the network).\n"
        "\tCPF_PrivateWrite =\t\t\t\t\t\t0x0000004000000000, // Property is const "
        "outside of the class it was declared in.\n"
        "\tCPF_ProtectedWrite =\t\t\t\t\t0x0000008000000000, // Property is const "
        "outside of the class it was declared in and subclasses.\n"
        "\tCPF_ArchetypeProperty =\t\t\t\t\t0x0000010000000000, // Property should "
        "be ignored by archives which have ArIgnoreArchetypeRef set.\n"
        "\tCPF_EditHide =\t\t\t\t\t\t\t0x0000020000000000, // Property should "
        "never be shown in a properties window.\n"
        "\tCPF_EditTextBox =\t\t\t\t\t\t0x0000040000000000, // Property can be "
        "edited using a text dialog box.\n"
        "\tCPF_CrossLevelPassive =\t\t\t\t\t0x0000100000000000, // Property can "
        "point across levels, and will be serialized properly, but assumes it\'s "
        "target exists in-game (non-editor)\n"
        "\tCPF_CrossLevelActive =\t\t\t\t\t0x0000200000000000, // Property can "
        "point across levels, and will be serialized properly, and will be updated "
        "when the target is streamed in/out\n"
        "};\n"
        "\n"
        "// "
        "https://github.com/CodeRedModding/UnrealEngine3/blob/main/Development/Src/"
        "Core/Inc/UnObjBas.h#L316\n"
        "// Object Flags\n"
        "enum EObjectFlags : uint64_t\n"
        "{\n"
        "\tRF_NoFlags =\t\t\t\t\t\t\t0x000000000000000,\t// Object has no flags.\n"
        "\tRF_InSingularFunc =\t\t\t\t\t\t0x0000000000000002,\t// In a singular "
        "function.\n"
        "\tRF_StateChanged =\t\t\t\t\t\t0x0000000000000004,\t// Object did a state "
        "change.\n"
        "\tRF_DebugPostLoad =\t\t\t\t\t\t0x0000000000000008,\t// For debugging "
        "PostLoad calls.\n"
        "\tRF_DebugSerialize =\t\t\t\t\t\t0x0000000000000010,\t// For debugging "
        "Serialize calls.\n"
        "\tRF_DebugFinishDestroyed =\t\t\t\t0x0000000000000020,\t// For debugging "
        "FinishDestroy calls.\n"
        "\tRF_EdSelected =\t\t\t\t\t\t\t0x0000000000000040,\t// Object is selected "
        "in one of the editors browser windows.\n"
        "\tRF_ZombieComponent =\t\t\t\t\t0x0000000000000080,\t// This component\'s "
        "template was deleted, so should not be used.\n"
        "\tRF_Protected =\t\t\t\t\t\t\t0x0000000000000100, // Property is "
        "protected (may only be accessed from its owner class or subclasses).\n"
        "\tRF_ClassDefaultObject =\t\t\t\t\t0x0000000000000200,\t// this object is "
        "its class\'s default object.\n"
        "\tRF_ArchetypeObject =\t\t\t\t\t0x0000000000000400, // this object is a "
        "template for another object (treat like a class default object).\n"
        "\tRF_ForceTagExp =\t\t\t\t\t\t0x0000000000000800, // Forces this object "
        "to be put into the export table when saving a package regardless of "
        "outer.\n"
        "\tRF_TokenStreamAssembled =\t\t\t\t0x0000000000001000, // Set if "
        "reference token stream has already been assembled.\n"
        "\tRF_MisalignedObject =\t\t\t\t\t0x0000000000002000, // Object\'s size no "
        "longer matches the size of its C++ class (only used during make, for "
        "native classes whose properties have changed).\n"
        "\tRF_RootSet =\t\t\t\t\t\t\t0x0000000000004000, // Object will not be "
        "garbage collected, even if unreferenced.\n"
        "\tRF_BeginDestroyed =\t\t\t\t\t\t0x0000000000008000,\t// BeginDestroy has "
        "been called on the object.\n"
        "\tRF_FinishDestroyed =\t\t\t\t\t0x0000000000010000, // FinishDestroy has "
        "been called on the object.\n"
        "\tRF_DebugBeginDestroyed =\t\t\t\t0x0000000000020000, // Whether object "
        "is rooted as being part of the root set (garbage collection).\n"
        "\tRF_MarkedByCooker =\t\t\t\t\t\t0x0000000000040000,\t// Marked by "
        "content cooker.\n"
        "\tRF_LocalizedResource =\t\t\t\t\t0x0000000000080000, // Whether resource "
        "object is localized.\n"
        "\tRF_InitializedProps =\t\t\t\t\t0x0000000000100000, // whether "
        "InitProperties has been called on this object\n"
        "\tRF_PendingFieldPatches =\t\t\t\t0x0000000000200000, // @script patcher: "
        "indicates that this struct will receive additional member properties from "
        "the script patcher.\n"
        "\tRF_IsCrossLevelReferenced =\t\t\t\t0x0000000000400000,\t// This object "
        "has been pointed to by a cross-level reference, and therefore requires "
        "additional cleanup upon deletion.\n"
        "\tRF_Saved =\t\t\t\t\t\t\t\t0x0000000080000000, // Object has been saved "
        "via SavePackage (temporary).\n"
        "\tRF_Transactional =\t\t\t\t\t\t0x0000000100000000, // Object is "
        "transactional.\n"
        "\tRF_Unreachable =\t\t\t\t\t\t0x0000000200000000, // Object is not "
        "reachable on the object graph.\n"
        "\tRF_Public =\t\t\t\t\t\t\t\t0x0000000400000000, // Object is visible "
        "outside its package.\n"
        "\tRF_TagImp =\t\t\t\t\t\t\t\t0x0000000800000000,\t// Temporary import tag "
        "in load/save.\n"
        "\tRF_TagExp =\t\t\t\t\t\t\t\t0x0000001000000000,\t// Temporary export tag "
        "in load/save.\n"
        "\tRF_Obsolete =\t\t\t\t\t\t\t0x0000002000000000, // Object marked as "
        "obsolete and should be replaced.\n"
        "\tRF_TagGarbage =\t\t\t\t\t\t\t0x0000004000000000,\t// Check during "
        "garbage collection.\n"
        "\tRF_DisregardForGC =\t\t\t\t\t\t0x0000008000000000,\t// Object is being "
        "disregard for GC as its static and itself and all references are always "
        "loaded.\n"
        "\tRF_PerObjectLocalized =\t\t\t\t\t0x0000010000000000,\t// Object is "
        "localized by instance name, not by class.\n"
        "\tRF_NeedLoad =\t\t\t\t\t\t\t0x0000020000000000, // During load, "
        "indicates object needs loading.\n"
        "\tRF_AsyncLoading =\t\t\t\t\t\t0x0000040000000000, // Object is being "
        "asynchronously loaded.\n"
        "\tRF_NeedPostLoadSubobjects =\t\t\t\t0x0000080000000000, // During load, "
        "indicates that the object still needs to instance subobjects and fixup "
        "serialized component references.\n"
        "\tRF_Suppress =\t\t\t\t\t\t\t0x0000100000000000, // @warning: Mirrored in "
        "UnName.h. Suppressed log name.\n"
        "\tRF_InEndState =\t\t\t\t\t\t\t0x0000200000000000, // Within an EndState "
        "call.\n"
        "\tRF_Transient =\t\t\t\t\t\t\t0x0000400000000000, // Don\'t save object.\n"
        "\tRF_Cooked =\t\t\t\t\t\t\t\t0x0000800000000000, // Whether the object "
        "has already been cooked\n"
        "\tRF_LoadForClient =\t\t\t\t\t\t0x0001000000000000, // In-file load for "
        "client.\n"
        "\tRF_LoadForServer =\t\t\t\t\t\t0x0002000000000000, // In-file load for "
        "client.\n"
        "\tRF_LoadForEdit =\t\t\t\t\t\t0x0004000000000000, // In-file load for "
        "client.\n"
        "\tRF_Standalone =\t\t\t\t\t\t\t0x0008000000000000,\t// Keep object around "
        "for editing even if unreferenced.\n"
        "\tRF_NotForClient =\t\t\t\t\t\t0x0010000000000000, // Don\'t load this "
        "object for the game client.\n"
        "\tRF_NotForServer =\t\t\t\t\t\t0x0020000000000000, // Don\'t load this "
        "object for the game server.\n"
        "\tRF_NotForEdit =\t\t\t\t\t\t\t0x0040000000000000,\t// Don\'t load this "
        "object for the editor.\n"
        "\tRF_NeedPostLoad =\t\t\t\t\t\t0x0100000000000000, // Object needs to be "
        "postloaded.\n"
        "\tRF_HasStack =\t\t\t\t\t\t\t0x0200000000000000, // Has execution stack.\n"
        "\tRF_Native =\t\t\t\t\t\t\t\t0x0400000000000000, // Native (UClass only)\n"
        "\tRF_Marked =\t\t\t\t\t\t\t\t0x0800000000000000,\t// Marked (for "
        "debugging).\n"
        "\tRF_ErrorShutdown =\t\t\t\t\t\t0x1000000000000000, // ShutdownAfterError "
        "called.\n"
        "\tRF_PendingKill =\t\t\t\t\t\t0x2000000000000000, // Objects that are "
        "pending destruction (invalid for gameplay but valid objects).\n"
        "\tRF_MarkedByCookerTemp =\t\t\t\t\t0x4000000000000000,\t// Temporarily "
        "marked by content cooker (should be cleared).\n"
        "\tRF_CookedStartupObject =\t\t\t\t0x8000000000000000, // This object was "
        "cooked into a startup package.\n"
        "\n"
        "\tRF_ContextFlags =\t\t\t\t\t\t(RF_NotForClient | RF_NotForServer | "
        "RF_NotForEdit), // All context flags.\n"
        "\tRF_LoadContextFlags =\t\t\t\t\t(RF_LoadForClient | RF_LoadForServer | "
        "RF_LoadForEdit), // Flags affecting loading.\n"
        "\tRF_Load =\t\t\t\t\t\t\t\t(RF_ContextFlags | RF_LoadContextFlags | "
        "RF_Public | RF_Standalone | RF_Native | RF_Obsolete | RF_Protected | "
        "RF_Transactional | RF_HasStack | RF_PerObjectLocalized | "
        "RF_ClassDefaultObject | RF_ArchetypeObject | RF_LocalizedResource), // "
        "Flags to load from Unrealfiles.\n"
        "\tRF_Keep =\t\t\t\t\t\t\t\t(RF_Native | RF_Marked | RF_PerObjectLocalized "
        "| RF_MisalignedObject | RF_DisregardForGC | RF_RootSet | "
        "RF_LocalizedResource), // Flags to persist across loads.\n"
        "\tRF_ScriptMask =\t\t\t\t\t\t\t(RF_Transactional | RF_Public | "
        "RF_Transient | RF_NotForClient | RF_NotForServer | RF_NotForEdit | "
        "RF_Standalone), // Script-accessible flags.\n"
        "\tRF_UndoRedoMask =\t\t\t\t\t\t(RF_PendingKill), // Undo/ redo will "
        "store/ restore these\n"
        "\tRF_PropagateToSubObjects =\t\t\t\t(RF_Public | RF_ArchetypeObject | "
        "RF_Transactional), // Sub-objects will inherit these flags from their "
        "SuperObject.\n"
        "\n"
        "\tRF_AllFlags =\t\t\t\t\t\t\t0xFFFFFFFFFFFFFFFF,\n"
        "};\n"
        "\n"
        "// "
        "https://github.com/CodeRedModding/UnrealEngine3/blob/main/Development/Src/"
        "Core/Inc/UnObjBas.h#L51\n"
        "// Package Flags\n"
        "enum EPackageFlags : uint32_t\n"
        "{\n"
        "\tPKG_AllowDownload =\t\t\t\t\t\t0x00000001,\t// Allow downloading "
        "package.\n"
        "\tPKG_ClientOptional =\t\t\t\t\t0x00000002,\t// Purely optional for "
        "clients.\n"
        "\tPKG_ServerSideOnly =\t\t\t\t\t0x00000004, // Only needed on the server "
        "side.\n"
        "\tPKG_Cooked =\t\t\t\t\t\t\t0x00000008,\t// Whether this package has been "
        "cooked for the target platform.\n"
        "\tPKG_Unsecure =\t\t\t\t\t\t\t0x00000010, // Not trusted.\n"
        "\tPKG_SavedWithNewerVersion =\t\t\t\t0x00000020,\t// Package was saved "
        "with newer version.\n"
        "\tPKG_Need =\t\t\t\t\t\t\t\t0x00008000,\t// Client needs to download this "
        "package.\n"
        "\tPKG_Compiling =\t\t\t\t\t\t\t0x00010000,\t// package is currently being "
        "compiled\n"
        "\tPKG_ContainsMap =\t\t\t\t\t\t0x00020000,\t// Set if the package "
        "contains a ULevel/ UWorld object\n"
        "\tPKG_Trash =\t\t\t\t\t\t\t\t0x00040000,\t// Set if the package was "
        "loaded from the trashcan\n"
        "\tPKG_DisallowLazyLoading =\t\t\t\t0x00080000,\t// Set if the archive "
        "serializing this package cannot use lazy loading\n"
        "\tPKG_PlayInEditor =\t\t\t\t\t\t0x00100000,\t// Set if the package was "
        "created for the purpose of PIE\n"
        "\tPKG_ContainsScript =\t\t\t\t\t0x00200000,\t// Package is allowed to "
        "contain UClasses and unrealscript\n"
        "\tPKG_ContainsDebugInfo =\t\t\t\t\t0x00400000,\t// Package contains debug "
        "info (for UDebugger)\n"
        "\tPKG_RequireImportsAlreadyLoaded =\t\t0x00800000,\t// Package requires "
        "all its imports to already have been loaded\n"
        "\tPKG_StoreCompressed =\t\t\t\t\t0x02000000,\t// Package is being stored "
        "compressed, requires archive support for compression\n"
        "\tPKG_StoreFullyCompressed =\t\t\t\t0x04000000,\t// Package is serialized "
        "normally, and then fully compressed after (must be decompressed before "
        "LoadPackage is called)\n"
        "\tPKG_ContainsFaceFXData =\t\t\t\t0x10000000,\t// Package contains FaceFX "
        "assets and/or animsets\n"
        "\tPKG_NoExportAllowed =\t\t\t\t\t0x20000000,\t// Package was NOT created "
        "by a modder.  Internal data not for export\n"
        "\tPKG_StrippedSource =\t\t\t\t\t0x40000000,\t// Source has been removed "
        "to compress the package size\n"
        "\tPKG_FilterEditorOnly =\t\t\t\t\t0x80000000,\t// Package has editor-only "
        "data filtered\n"
        "};\n"
        "\n"
        "// "
        "https://github.com/CodeRedModding/UnrealEngine3/blob/"
        "7bf53e29f620b0d4ca5c9bd063a2d2dbcee732fe/Development/Src/Core/Inc/"
        "UnObjBas.h#L98\n"
        "// Class Flags\n"
        "enum EClassFlags : uint32_t\n"
        "{\n"
        "\tCLASS_None =\t\t\t\t\t\t\t0x00000000, \n"
        "\tCLASS_Abstract =\t\t\t\t\t\t0x00000001, // Class is abstract and can\'t "
        "be instantiated directly.\n"
        "\tCLASS_Compiled =\t\t\t\t\t\t0x00000002, // Script has been compiled "
        "successfully.\n"
        "\tCLASS_Config =\t\t\t\t\t\t\t0x00000004, // Load object configuration at "
        "construction time.\n"
        "\tCLASS_Transient =\t\t\t\t\t\t0x00000008, // This object type can\'t be "
        "saved; null it out at save time.\n"
        "\tCLASS_Parsed =\t\t\t\t\t\t\t0x00000010, // Successfully parsed.\n"
        "\tCLASS_Localized =\t\t\t\t\t\t0x00000020, // Class contains localized "
        "text.\n"
        "\tCLASS_SafeReplace =\t\t\t\t\t\t0x00000040, // Objects of this class can "
        "be safely replaced with default or NULL.\n"
        "\tCLASS_Native =\t\t\t\t\t\t\t0x00000080, // Class is a native class - "
        "native interfaces will have CLASS_Native set, but not RF_Native.\n"
        "\tCLASS_NoExport =\t\t\t\t\t\t0x00000100, // Don\'t export to C++ "
        "header.\n"
        "\tCLASS_Placeable =\t\t\t\t\t\t0x00000200, // Allow users to create in "
        "the editor.\n"
        "\tCLASS_PerObjectConfig =\t\t\t\t\t0x00000400, // Handle object "
        "configuration on a per-object basis, rather than per-class.\n"
        "\tCLASS_NativeReplication =\t\t\t\t0x00000800, // Replication handled in "
        "C++.\n"
        "\tCLASS_EditInlineNew =\t\t\t\t\t0x00001000, // Class can be constructed "
        "from editinline New button..\n"
        "\tCLASS_CollapseCategories =\t\t\t\t0x00002000,\t// Display properties in "
        "the editor without using categories.\n"
        "\tCLASS_Interface =\t\t\t\t\t\t0x00004000, // Class is an interface.\n"
        "\tCLASS_HasInstancedProps =\t\t\t\t0x00200000, // class contains object "
        "properties which are marked \"instanced\" (or editinline export).\n"
        "\tCLASS_NeedsDefProps =\t\t\t\t\t0x00400000, // Class needs its "
        "defaultproperties imported.\n"
        "\tCLASS_HasComponents =\t\t\t\t\t0x00800000, // Class has component "
        "properties.\n"
        "\tCLASS_Hidden =\t\t\t\t\t\t\t0x01000000, // Don\'t show this class in "
        "the editor class browser or edit inline new menus.\n"
        "\tCLASS_Deprecated =\t\t\t\t\t\t0x02000000, // Don\'t save objects of "
        "this class when serializing.\n"
        "\tCLASS_HideDropDown =\t\t\t\t\t0x04000000, // Class not shown in editor "
        "drop down for class selection.\n"
        "\tCLASS_Exported =\t\t\t\t\t\t0x08000000, // Class has been exported to a "
        "header file.\n"
        "\tCLASS_Intrinsic =\t\t\t\t\t\t0x10000000, // Class has no unrealscript "
        "counter-part.\n"
        "\tCLASS_NativeOnly =\t\t\t\t\t\t0x20000000, // Properties in this class "
        "can only be accessed from native code.\n"
        "\tCLASS_PerObjectLocalized =\t\t\t\t0x40000000, // Handle object "
        "localization on a per-object basis, rather than per-class. \n"
        "\tCLASS_HasCrossLevelRefs =\t\t\t\t0x80000000, // This class has "
        "properties that are marked with CPF_CrossLevel \n"
        "\n"
        "\t// Deprecated, these values now match the values of the EClassCastFlags "
        "enum.\n"
        "\tCLASS_IsAUProperty =\t\t\t\t\t0x00008000,\n"
        "\tCLASS_IsAUObjectProperty =\t\t\t\t0x00010000,\n"
        "\tCLASS_IsAUBoolProperty =\t\t\t\t0x00020000,\n"
        "\tCLASS_IsAUState =\t\t\t\t\t\t0x00040000,\n"
        "\tCLASS_IsAUFunction =\t\t\t\t\t0x00080000,\n"
        "\tCLASS_IsAUStructProperty =\t\t\t\t0x00100000,\n"
        "\n"
        "\t// Flags to inherit from base class.\n"
        "\tCLASS_Inherit =\t\t\t\t\t\t\t(CLASS_Transient | CLASS_Config | "
        "CLASS_Localized | CLASS_SafeReplace | CLASS_PerObjectConfig | "
        "CLASS_PerObjectLocalized | CLASS_Placeable | CLASS_IsAUProperty | "
        "CLASS_IsAUObjectProperty | CLASS_IsAUBoolProperty | "
        "CLASS_IsAUStructProperty | CLASS_IsAUState | CLASS_IsAUFunction | "
        "CLASS_HasComponents | CLASS_Deprecated | CLASS_Intrinsic | "
        "CLASS_HasInstancedProps | CLASS_HasCrossLevelRefs),\n"
        "\n"
        "\t// These flags will be cleared by the compiler when the class is parsed "
        "during script compilation.\n"
        "\tCLASS_RecompilerClear =\t\t\t\t\t(CLASS_Inherit | CLASS_Abstract | "
        "CLASS_NoExport | CLASS_NativeReplication | CLASS_Native),\n"
        "\n"
        "\t// These flags will be inherited from the base class only for "
        "non-intrinsic classes.\n"
        "\tCLASS_ScriptInherit =\t\t\t\t\t(CLASS_Inherit | CLASS_EditInlineNew | "
        "CLASS_CollapseCategories),\n"
        "\n"
        "\tCLASS_AllFlags =\t\t\t\t\t\t0xFFFFFFFF,\n"
        "};\n"
        "\n"
        "// "
        "https://github.com/CodeRedModding/UnrealEngine3/blob/"
        "7bf53e29f620b0d4ca5c9bd063a2d2dbcee732fe/Development/Src/Core/Inc/"
        "UnObjBas.h#L195\n"
        "// Class Cast Flags\n"
        "enum EClassCastFlag : uint32_t\n"
        "{\n"
        "\tCASTCLASS_None =\t\t\t\t\t\t0x00000000,\n"
        "\tCASTCLASS_UField =\t\t\t\t\t\t0x00000001,\n"
        "\tCASTCLASS_UConst =\t\t\t\t\t\t0x00000002,\n"
        "\tCASTCLASS_UEnum =\t\t\t\t\t\t0x00000004,\n"
        "\tCASTCLASS_UStruct =\t\t\t\t\t\t0x00000008,\n"
        "\tCASTCLASS_UScriptStruct =\t\t\t\t0x00000010,\n"
        "\tCASTCLASS_UClass =\t\t\t\t\t\t0x00000020,\n"
        "\tCASTCLASS_UByteProperty =\t\t\t\t0x00000040,\n"
        "\tCASTCLASS_UIntProperty =\t\t\t\t0x00000080,\n"
        "\tCASTCLASS_UFloatProperty =\t\t\t\t0x00000100,\n"
        "\tCASTCLASS_UComponentProperty =\t\t\t0x00000200,\n"
        "\tCASTCLASS_UClassProperty =\t\t\t\t0x00000400,\n"
        "\tCASTCLASS_UInterfaceProperty =\t\t\t0x00001000,\n"
        "\tCASTCLASS_UNameProperty =\t\t\t\t0x00002000,\n"
        "\tCASTCLASS_UStrProperty =\t\t\t\t0x00004000,\n"
        "\n"
        "\t// These match the values of the old class flags to make conversion "
        "easier.\n"
        "\tCASTCLASS_UProperty =\t\t\t\t\t0x00008000,\n"
        "\tCASTCLASS_UObjectProperty =\t\t\t\t0x00010000,\n"
        "\tCASTCLASS_UBoolProperty =\t\t\t\t0x00020000,\n"
        "\tCASTCLASS_UState =\t\t\t\t\t\t0x00040000,\n"
        "\tCASTCLASS_UFunction =\t\t\t\t\t0x00080000,\n"
        "\tCASTCLASS_UStructProperty =\t\t\t\t0x00100000,\n"
        "\n"
        "\tCASTCLASS_UArrayProperty =\t\t\t\t0x00200000,\n"
        "\tCASTCLASS_UMapProperty =\t\t\t\t0x00400000,\n"
        "\tCASTCLASS_UDelegateProperty =\t\t\t0x00800000,\n"
        "\tCASTCLASS_UComponent =\t\t\t\t\t0x01000000,\n"
        "\n"
        "\tCASTCLASS_AllFlags =\t\t\t\t\t0xFFFFFFFF,\n"
        "};\n";
} // namespace PiecesOfCode

namespace PiecesOfTypes {
    const std::string Example_Struct = "struct FExampleStruct\n"
        "{\n"
        "\tstruct FQuat Quaternion;\n"
        "\tstruct FVector Location;\n"
        "\tstruct FVector LinearVelocity;\n"
        "\tstruct FVector AngularVelocity;\n"
        "};\n\n";
}

/*
#
=========================================================================================
#
#
#
=========================================================================================
#
*/
