#ifndef SUBSCRIPTIONS_H
#define SUBSCRIPTIONS_H

#include <iostream>
#include <vector>
#include <list>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

class sub {
protected:
    int Price;
    int Tier;
    wstring Region;
    wstring Type;
    bool virtualOrPhys;

public:
    sub(int price, int tier, const wstring& region, const wstring& type)
        : Price(price), Tier(tier), Region(region), Type(type), virtualOrPhys(false) {}
    virtual ~sub() = default;

    virtual void Buying() = 0;
    virtual void Connecting() = 0;
    virtual void Downloading() = 0;
    virtual void ShowInfo() = 0;

    int GetPrice() const { return Price; }
    int GetTier() const { return Tier; }
    wstring GetRegion() const { return Region; }
};

enum class SubscriptionType { GAME_PASS, PS_PLUS, UBI_PLUS };

class GamePass : public sub {
public:
    GamePass(int price, int tier, const wstring& region, const wstring& type)
        : sub(price, tier, region, type) {
        virtualOrPhys = true;
    }

    void Buying() override { wcout << L"Покупаем Game Pass" << endl; }
    void Connecting() override { wcout << L"Активируем Game Pass" << endl; }
    void Downloading() override { wcout << L"Скачиваем игры через Game Pass" << endl; }
    void ShowInfo() override {
        wcout << L"=== GAME PASS ===" << endl
            << L"Цена: " << Price << L" руб." << endl
            << L"Уровень: " << Tier << endl
            << L"Регион: " << Region << endl
            << L"Тип: " << Type << endl;
    }
};

class PsPlus : public sub {
public:
    PsPlus(int price, int tier, const wstring& region, const wstring& type)
        : sub(price, tier, region, type) {
        virtualOrPhys = true;
    }

    void Buying() override { wcout << L"Покупаем PS Plus" << endl; }
    void Connecting() override { wcout << L"Активируем PS Plus" << endl; }
    void Downloading() override { wcout << L"Скачиваем игры через PS Plus" << endl; }
    void ShowInfo() override {
        wcout << L"=== PS PLUS ===" << endl
            << L"Цена: " << Price << L" руб." << endl
            << L"Уровень: " << Tier << endl
            << L"Регион: " << Region << endl
            << L"Тип: " << Type << endl;
    }
};

class UbiPlus : public sub {
public:
    UbiPlus(int price, int tier, const wstring& region, const wstring& type)
        : sub(price, tier, region, type) {
        virtualOrPhys = false;
    }

    void Buying() override { wcout << L"Покупаем Ubisoft+" << endl; }
    void Connecting() override { wcout << L"Активируем Ubisoft+" << endl; }
    void Downloading() override { wcout << L"Скачиваем игры через Ubisoft+" << endl; }
    void ShowInfo() override {
        wcout << L"=== UBISOFT+ ===" << endl
            << L"Цена: " << Price << L" руб." << endl
            << L"Уровень: " << Tier << endl
            << L"Регион: " << Region << endl
            << L"Тип: " << Type << endl;
    }
};

sub* CreateSubscription(SubscriptionType type, int price, int tier,
    const wstring& region, const wstring& typeStr) {
    switch (type) {
    case SubscriptionType::GAME_PASS: return new GamePass(price, tier, region, typeStr);
    case SubscriptionType::PS_PLUS: return new PsPlus(price, tier, region, typeStr);
    case SubscriptionType::UBI_PLUS: return new UbiPlus(price, tier, region, typeStr);
    default: return nullptr;
    }
}

template<typename T>
class Iterator {
public:
    virtual ~Iterator() = default;
    virtual void First() = 0;
    virtual void Next() = 0;
    virtual bool IsDone() const = 0;
    virtual T GetCurrent() const = 0;
};

template<typename T>
class Container {
public:
    virtual ~Container() = default;
    virtual Iterator<T>* GetIterator() = 0;
    virtual size_t Size() const = 0;
};

template<typename T>
class ArrayContainer : public Container<T> {
    static const size_t MAX_SIZE = 100;
    T items[MAX_SIZE];
    size_t count = 0;

public:
    void Add(T item) { if (count < MAX_SIZE) items[count++] = item; }
    T Get(size_t index) const { return items[index]; }
    size_t Size() const override { return count; }

    Iterator<T>* GetIterator() override;
};

template<typename T>
class ArrayIterator : public Iterator<T> {
    const ArrayContainer<T>* container;
    size_t current = 0;

public:
    explicit ArrayIterator(const ArrayContainer<T>* cont) : container(cont) {}
    void First() override { current = 0; }
    void Next() override { current++; }
    bool IsDone() const override { return current >= container->Size(); }
    T GetCurrent() const override { return container->Get(current); }
};

template<typename T>
Iterator<T>* ArrayContainer<T>::GetIterator() {
    return new ArrayIterator<T>(this);
}

template<typename T>
class ListContainer : public Container<T> {
    list<T> items;

public:
    void Add(T item) { items.push_back(item); }
    const list<T>& GetItems() const { return items; }
    size_t Size() const override { return items.size(); }

    Iterator<T>* GetIterator() override;
};

template<typename T>
class ListIterator : public Iterator<T> {
    typename list<T>::const_iterator current;
    typename list<T>::const_iterator end;
    const ListContainer<T>* container;

public:
    explicit ListIterator(const ListContainer<T>* cont)
        : container(cont), current(cont->GetItems().begin()), end(cont->GetItems().end()) {}
    void First() override { current = container->GetItems().begin(); }
    void Next() override { ++current; }
    bool IsDone() const override { return current == end; }
    T GetCurrent() const override { return *current; }
};

template<typename T>
Iterator<T>* ListContainer<T>::GetIterator() {
    return new ListIterator<T>(this);
}

template<typename T>
class PriceFilterIterator : public Iterator<T> {
    Iterator<T>* it;
    int minPrice;
    int maxPrice;

public:
    PriceFilterIterator(Iterator<T>* iterator, int min, int max)
        : it(iterator), minPrice(min), maxPrice(max) {}

    ~PriceFilterIterator() { delete it; }

    void First() override {
        it->First();
        SkipUnwanted();
    }

    void Next() override {
        it->Next();
        SkipUnwanted();
    }

    bool IsDone() const override { return it->IsDone(); }
    T GetCurrent() const override { return it->GetCurrent(); }

private:
    void SkipUnwanted() {
        while (!it->IsDone()) {
            sub* s = it->GetCurrent();
            if (s->GetPrice() >= minPrice && s->GetPrice() <= maxPrice) {
                break;
            }
            it->Next();
        }
    }
};

template<typename T>
class TierFilterIterator : public Iterator<T> {
    Iterator<T>* it;
    int tier;

public:
    TierFilterIterator(Iterator<T>* iterator, int t)
        : it(iterator), tier(t) {}

    ~TierFilterIterator() { delete it; }

    void First() override {
        it->First();
        SkipUnwanted();
    }

    void Next() override {
        it->Next();
        SkipUnwanted();
    }

    bool IsDone() const override { return it->IsDone(); }
    T GetCurrent() const override { return it->GetCurrent(); }

private:
    void SkipUnwanted() {
        while (!it->IsDone()) {
            sub* s = it->GetCurrent();
            if (s->GetTier() == tier) {
                break;
            }
            it->Next();
        }
    }
};

template<typename T>
class RegionFilterIterator : public Iterator<T> {
    Iterator<T>* it;
    wstring region;

public:
    RegionFilterIterator(Iterator<T>* iterator, const wstring& reg)
        : it(iterator), region(reg) {}

    ~RegionFilterIterator() { delete it; }

    void First() override {
        it->First();
        SkipUnwanted();
    }

    void Next() override {
        it->Next();
        SkipUnwanted();
    }

    bool IsDone() const override { return it->IsDone(); }
    T GetCurrent() const override { return it->GetCurrent(); }

private:
    void SkipUnwanted() {
        while (!it->IsDone()) {
            sub* s = it->GetCurrent();
            if (s->GetRegion() == region) {
                break;
            }
            it->Next();
        }
    }
};

template<typename ContainerType, typename ItemType>
class STLIteratorAdapter : public Iterator<ItemType> {
    typename ContainerType::const_iterator current;
    typename ContainerType::const_iterator end;
    const ContainerType* container;

public:
    STLIteratorAdapter(const ContainerType* cont)
        : container(cont), current(cont->begin()), end(cont->end()) {}

    void First() override { current = container->begin(); }
    void Next() override { ++current; }
    bool IsDone() const override { return current == end; }
    ItemType GetCurrent() const override { return *current; }
};

void ProcessSubscriptions(Iterator<sub*>* iterator) {
    for (iterator->First(); !iterator->IsDone(); iterator->Next()) {
        sub* s = iterator->GetCurrent();
        s->ShowInfo();
        s->Buying();
        s->Connecting();
        s->Downloading();
        wcout << L"---------------------" << endl;
    }
}

#endif 
