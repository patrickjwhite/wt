#include <Wt/WPushButton>
#include <Wt/WTemplate>

SAMPLE_BEGIN(PushButtonSize)
Wt::WTemplate *result =
    new Wt::WTemplate(Wt::WString::tr("pushButtonSize-template"));

Wt::WPushButton *button = new Wt::WPushButton("Large");
button->setStyleClass("btn-large");
result->bindWidget("button-large", button);

button = new Wt::WPushButton("Default");
result->bindWidget("button-default", button);

button = new Wt::WPushButton("Small");
button->setStyleClass("btn-small");
result->bindWidget("button-small", button);

button = new Wt::WPushButton("Mini");
button->setStyleClass("btn-mini");
result->bindWidget("button-mini", button);

SAMPLE_END(return result)
