#ifndef SUBSCRIPTIONS_H
#define SUBSCRIPTIONS_H

#include <iostream>
#include <vector>
#include <list>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

// Паттерн "Стратегия" - стратегии активации подписки
enum class ActivationStrategyEnum {
    Standard,
    Premium,
    Trial,
    None
};

class ActivationStrategy {
public:
    virtual ~ActivationStrategy() {}
    virtual void Activate() = 0;
};

class StandardActivation : public ActivationStrategy {
    void Activate() { wcout << L"Активация стандартной подписки" << endl; }
};

class PremiumActivation : public ActivationStrategy {
    void Activate() { wcout << L"Активация премиум подписки с бонусами" << endl; }
};

class TrialActivation : public ActivationStrategy {
    void Activate() { wcout << L"Активация пробной версии на 7 дней" << endl; }
};

ActivationStrategy* CreateActivationStrategy(ActivationStrategyEnum strategy) {
    switch(strategy) {
        case ActivationStrategyEnum::Standard: return new StandardActivation;
        case ActivationStrategyEnum::Premium: return new PremiumActivation;
        case ActivationStrategyEnum::Trial: return new TrialActivation;
        default: return nullptr;
    }
}

class sub {
protected:
    int Price;
    int Tier;
    wstring Region;
    wstring Type;
    ActivationStrategy* Activation;

    virtual void SpecificActivationSteps() = 0;
    virtual void ShowSpecificInfo() = 0;

    void CommonActivationSteps() {
        wcout << L"Проверка платежных данных..." << endl;
        wcout << L"Резервирование средств..." << endl;
    }

    void CommonInfo() {
        wcout << L"Цена: " << Price << L" руб." << endl;
        wcout << L"Уровень: " << Tier << endl;
        wcout << L"Регион: " << Region << endl;
    }

public:
    sub(int price, int tier, const wstring& region, const wstring& type)
        : Price(price), Tier(tier), Region(region), Type(type), Activation(nullptr) {}
    
    virtual ~sub() {
        if(Activation != nullptr) delete Activation;
    }

    // Шаблонный метод
    void Activate() {
        wcout << L"=== Начало активации ===" << endl;
        CommonActivationSteps();
        SpecificActivationSteps();
        if(Activation) Activation->Activate();
        wcout << L"=== Активация завершена ===" << endl << endl;
    }

    void ShowInfo() {
        wcout << L"=== Информация о подписке ===" << endl;
        CommonInfo();
        ShowSpecificInfo();
        wcout << endl;
    }

    void SetActivationStrategy(ActivationStrategy* strategy) {
        Activation = strategy;
    }
};

class GamePass : public sub {
protected:
    void SpecificActivationSteps() override {
        wcout << L"Проверка аккаунта Microsoft..." << endl;
    }
    
    void ShowSpecificInfo() override {
        wcout << L"Тип: Game Pass " << Type << endl;
    }

public:
    GamePass(int price, int tier, const wstring& region, const wstring& type)
        : sub(price, tier, region, type) {
        SetActivationStrategy(CreateActivationStrategy(ActivationStrategyEnum::Standard));
    }
};

class PsPlus : public sub {
protected:
    void SpecificActivationSteps() override {
        wcout << L"Проверка аккаунта PSN..." << endl;
    }
    
    void ShowSpecificInfo() override {
        wcout << L"Тип: PS Plus " << Type << endl;
    }

public:
    PsPlus(int price, int tier, const wstring& region, const wstring& type)
        : sub(price, tier, region, type) {
        SetActivationStrategy(CreateActivationStrategy(ActivationStrategyEnum::Premium));
    }
};

class UbiPlus : public sub {
protected:
    void SpecificActivationSteps() override {
        wcout << L"Проверка аккаунта Ubisoft..." << endl;
    }
    
    void ShowSpecificInfo() override {
        wcout << L"Тип: Ubisoft+ " << Type << endl;
    }

public:
    UbiPlus(int price, int tier, const wstring& region, const wstring& type)
        : sub(price, tier, region, type) {
        SetActivationStrategy(CreateActivationStrategy(ActivationStrategyEnum::Trial));
    }
};

enum class SubscriptionType { GAME_PASS, PS_PLUS, UBI_PLUS };

sub* CreateSubscription(SubscriptionType type, int price, int tier,
    const wstring& region, const wstring& typeStr) {
    switch(type) {
        case SubscriptionType::GAME_PASS: return new GamePass(price, tier, region, typeStr);
        case SubscriptionType::PS_PLUS: return new PsPlus(price, tier, region, typeStr);
        case SubscriptionType::UBI_PLUS: return new UbiPlus(price, tier, region, typeStr);
        default: return nullptr;
    }
}

#endif 
