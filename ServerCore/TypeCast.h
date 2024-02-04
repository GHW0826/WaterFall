#pragma once
#ifndef __TYPECAST__
#define __TYPECAST__

#include "Types.h"

#define DECLARE_TL using TL = TL; int32 _typeId;
#define INIT_TL(Type) _typeId = IndexOf<TL, Type>::value;

#pragma region TypeList

/*
	example)

	TypeList<Mage, Knight>::Head whoAMI;
	TypeList<Mage, Knight>::Tail whoAMI2;
	TypeList<Mage, Knight, Archer>::Tail::Head test;

	TypeList<Mage, TypeList<Knight, Archer>>::Head whoAMI3;
	TypeList<Mage, TypeList<Knight, Archer>>::Tail::Head whoAMI4;
	TypeList<Mage, TypeList<Knight, Archer>>::Tail::Tail whoAMI5;
*/
template<typename... T>
struct TypeList;

template<typename T, typename U>
struct TypeList<T, U> {
	using Head = T;
	using Tail = U;
};

template<typename T, typename... U>
struct TypeList<T, U...> {
	using Head = T;
	using Tail = TypeList<U...>;
};

#pragma endregion


#pragma region Length

/*
	example

	int32 len1 = Length<TypeList<Mage, Knight>>::value;
	int32 len2 = Length<TypeList<Mage, Knight, Archer>>::value;
*/

template<typename T>
struct Length;

template<>
struct Length<TypeList<>> {
	enum { value = 0 };
};

template <typename T, typename... U>
struct Length<TypeList<T, U...>> {
	enum { value = 1 + Length<TypeList<U...>>::value };
};

#pragma endregion



#pragma region TypeAt

/*
	example

	TypeAt<TL, 0>::Result whoAMI6;
	TypeAt<TL, 1>::Result whoAMI7;
	TypeAt<TL, 2>::Result whoAMI8;
*/

template<typename TL, int32 index>
struct TypeAt;

template<typename Head, typename... Tail>
struct TypeAt<TypeList<Head, Tail...>, 0> {
	using Result = Head;
};

template<typename Head, typename... Tail, int32 index>
struct TypeAt<TypeList<Head, Tail...>, index> {
	using Result = typename TypeAt<TypeList<Tail...>, index - 1>::Result;
};

#pragma endregion


#pragma region IndexOf
/*
	example

	int32 index1 = IndexOf<TL, Mage>::value;
	int32 index2 = IndexOf<TL, Archer>::value;
	int32 index3 = IndexOf<TL, Dog>::value;
*/

template<typename TL, typename T>
struct IndexOf;

template<typename... Tail, typename T>
struct IndexOf<TypeList<T, Tail...>, T> {
	enum { value = 0 };
};

template<typename T>
struct IndexOf<TypeList<>, T> {
	enum { value = -1 };
};

template<typename Head, typename... Tail, typename T>
struct IndexOf<TypeList<Head, Tail...>, T> {
private:
	enum { temp = IndexOf<TypeList<Tail...>, T>::value };
public:
	enum { value = (temp == -1) ? -1 : temp + 1 };
};

#pragma endregion


#pragma region Conversion
/*
	example

	bool canConvert1 = Conversion<Player, Knight>::exists;
	bool canConvert2 = Conversion<Knight, Player>::exists;
	bool canConvert3 = Conversion<Knight, Dog>::exists;
*/
template<typename From, typename To>
class Conversion {
private:
	using Small = __int8;
	using Big = __int32;

	static Small Test(const To&) { return 0; }
	static Big Test(...) { return 0; }
	static From MakeFrom() { return 0; }
public:
	enum {
		exists = sizeof(Test(MakeFrom())) == sizeof(Small)
	};
};

#pragma endregion


#pragma region TypeCast
/*
	exmaple
	{
		Player* player = new Knight();

		bool canCast = CanCast<Knight*>(player);
		Knight* knight = TypeCast<Knight*>(player);


		delete player;
	}

	{
		shared_ptr<Player> player = MakeShared<Knight>();

		shared_ptr<Archer> archer = TypeCast<Archer>(player);
		bool canCast = CanCast<Mage>(player);

	}
*/

template<int32 v>
struct Int2Type {
	enum { value = v };
};

template<typename TL>
class TypeConversion {
public:
	enum {
		length = Length<TL>::value
	};

	TypeConversion() {
		MakeTable(Int2Type<0>(), Int2Type<0>());
	}

	template<int32 i, int32 j>
	static void MakeTable(Int2Type<i>, Int2Type<j>) {
		using FromType = typename TypeAt<TL, i>::Result;
		using ToType = typename TypeAt<TL, j>::Result;
		if (Conversion<const FromType*, const ToType*>::exists)
			s_convert[i][j] = true;
		else
			s_convert[i][j] = false;

		MakeTable(Int2Type<i>(), Int2Type<j + 1>());
	}

	template<int32 i>
	static void MakeTable(Int2Type<i>, Int2Type<length>) {
		MakeTable(Int2Type<i + 1>(), Int2Type<0>());
	}

	template<int32 j>
	static void MakeTable(Int2Type<length>, Int2Type<j>)
	{}

	static inline bool CanConvert(int32 from, int32 to) {
		static TypeConversion conversion;
		return s_convert[from][to];
	}
public:
	static bool s_convert[length][length];
};

template<typename TL>
bool TypeConversion<TL>::s_convert[length][length];


template<typename To, typename From>
To TypeCast(From* ptr) {
	if (ptr == nullptr)
		return nullptr;

	using TL = typename From::TL;
	if (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value))
		return static_cast<To>(ptr);

	return nullptr;
}


template<typename To, typename From>
shared_ptr<To> TypeCast(shared_ptr<From> ptr) {
	if (ptr == nullptr)
		return nullptr;

	using TL = typename From::TL;
	if (TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value))
		return static_pointer_cast<To>(ptr);

	return nullptr;
}

template<typename To, typename From>
bool CanCast(From* ptr) {
	if (ptr == nullptr)
		return false;

	using TL = typename From::TL;
	return TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value);
}

template<typename To, typename From>
bool CanCast(shared_ptr<From> ptr) {
	if (ptr == nullptr)
		return false;

	using TL = typename From::TL;
	return TypeConversion<TL>::CanConvert(ptr->_typeId, IndexOf<TL, remove_pointer_t<To>>::value);
}

#pragma endregion



#endif