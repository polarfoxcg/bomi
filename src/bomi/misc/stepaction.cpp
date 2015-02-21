#include "stepaction.hpp"

StepAction::StepAction(ChangeValue t, QObject *parent)
    : EnumAction<ChangeValue>(t, parent)
{

}

auto StepAction::toString(bool sign, int value) const -> QString
{
    if (!m_textRate)
        return _NS(value, sign);
    int dec = 0;
    const auto abs = qAbs(value* m_textRate);
    if (abs < 0.01)
        dec = 3;
    else if (abs < 0.1)
        dec = 2;
    else if (abs < 1)
        dec = 1;
    return _NS(value*m_textRate, sign, dec);
}

auto StepAction::update() -> void
{
    const int data = m_step*EnumInfo::data(enum_());
    setData(data);
    if (!data)
        setText(EnumInfo::description(enum_()));
    else
        setText(m_format.arg(toString(true, data)));
}

auto StepAction::format(int value) const -> QString
{
    return m_format.arg(toString(m_min < 0 && 0 < m_max, value));
}
