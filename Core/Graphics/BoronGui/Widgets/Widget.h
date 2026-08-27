#pragma once

namespace Borongui {
class Widget {
public:
    virtual ~Widget() = default;
    virtual void Render() = 0;
};
}