#pragma once

#include <string>

namespace CG {

    class Resource {
    public:
        virtual ~Resource() = 0{}

        void SetName(const std::string& name) { name_ = name; }
        const std::string& GetName() const { return name_; }

    private:
        std::string name_;
    };

}