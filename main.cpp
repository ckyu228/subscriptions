#include "subscriptions.h"

int main() {
    setlocale(LC_ALL, "Russian");
    srand(time(0));

    sub* subscriptions[] = {
        CreateSubscription(SubscriptionType::GAME_PASS, 599, 1, L"Россия", L"Месячная"),
        CreateSubscription(SubscriptionType::PS_PLUS, 899, 2, L"США", L"Годовая"),
        CreateSubscription(SubscriptionType::UBI_PLUS, 399, 1, L"Европа", L"Квартальная")
    };

    for(sub* s : subscriptions) {
        s->ShowInfo();
        s->Activate();
    }

    wcout << L"\n=== Изменение стратегии активации ===\n";
    subscriptions[0]->SetActivationStrategy(CreateActivationStrategy(ActivationStrategyEnum::Premium));
    subscriptions[0]->Activate();

    for(sub* s : subscriptions) {
        delete s;
    }

    return 0;
}
