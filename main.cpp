#include "subscriptions.h"

int main() {
    setlocale(LC_ALL, "Russian");
    srand(time(0));

    ArrayContainer<sub*> arrayContainer;
    ListContainer<sub*> listContainer;

    for (int i = 0; i < 10; i++) {
        SubscriptionType type = static_cast<SubscriptionType>(rand() % 3);
        int price = 100 + rand() % 2000;
        int tier = 1 + rand() % 3;

        arrayContainer.Add(CreateSubscription(type, price, tier, L"Россия", L"Месячная"));
        listContainer.Add(CreateSubscription(type, price, tier, L"США", L"Годовая"));
    }

    wcout << L"\n=== Все подписки в контейнере ===" << endl;
    Iterator<sub*>* arrayIt = arrayContainer.GetIterator();
    ProcessSubscriptions(arrayIt);
    delete arrayIt;

    // Фильтрации по цене
    wcout << L"\n=== Подписки в контейнере (цена 500-1500 руб) ===" << endl;
    Iterator<sub*>* priceIt = new PriceFilterIterator<sub*>(arrayContainer.GetIterator(), 500, 1500);
    ProcessSubscriptions(priceIt);
    delete priceIt;

    // Фильтрации по уровню подписки
    wcout << L"\n=== Подписки в контейнере (с уровнем 2) ===" << endl;
    Iterator<sub*>* tierIt = new TierFilterIterator<sub*>(listContainer.GetIterator(), 2);
    ProcessSubscriptions(tierIt);
    delete tierIt;

    // Фильтрации по региону 
    wcout << L"\n=== Подписки в контейнере с регионом Россия ===" << endl;
    Iterator<sub*>* regionIt = new RegionFilterIterator<sub*>(arrayContainer.GetIterator(), L"Россия");
    ProcessSubscriptions(regionIt);
    delete regionIt;

    // Фильтрация по цене и уровню
    wcout << L"\n=== Подписки в контейнере (цена 500-1500 руб, Уровень 3) ===" << endl;
    Iterator<sub*>* combinedIt = new TierFilterIterator<sub*>(
        new PriceFilterIterator<sub*>(listContainer.GetIterator(), 500, 1500), 3);
    ProcessSubscriptions(combinedIt);
    delete combinedIt;

    // Адаптер для итератора в стиле STL
    vector<sub*> stlVector;
    for (int i = 0; i < 5; i++) {
        SubscriptionType type = static_cast<SubscriptionType>(rand() % 3);
        int price = 100 + rand() % 2000;
        int tier = 1 + rand() % 3;
        stlVector.push_back(CreateSubscription(type, price, tier, L"Европа", L"Квартальная"));
    }

    wcout << L"\n=== Подписки в STL vector через адаптер ===" << endl;
    Iterator<sub*>* stlIt = new STLIteratorAdapter<vector<sub*>, sub*>(&stlVector);
    ProcessSubscriptions(stlIt);
    delete stlIt;

    for (size_t i = 0; i < arrayContainer.Size(); i++) delete arrayContainer.Get(i);

    Iterator<sub*>* cleanIt = listContainer.GetIterator();
    for (cleanIt->First(); !cleanIt->IsDone(); cleanIt->Next()) {
        delete cleanIt->GetCurrent();
    }
    delete cleanIt;

    for (auto item : stlVector) delete item;

    return 0;
}
