/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2017, The Souffle Developers. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file ExplainProvenance.h
 *
 * Abstract class for implementing an instance of the explain interface for provenance
 *
 ***********************************************************************/

#pragma once

#include "ExplainTree.h"
#include "RamTypes.h"
#include "SouffleInterface.h"

#include <sstream>
#include <string>
#include <vector>

namespace souffle {

/** utility function to split a string */
inline std::vector<std::string> split(std::string s, char delim, int times = -1) {
    std::vector<std::string> v;
    std::stringstream ss(s);
    std::string item;

    while ((times > 0 || times <= -1) && std::getline(ss, item, delim)) {
        v.push_back(item);
        times--;
    }

    if (ss.peek() != EOF) {
        std::string remainder;
        std::getline(ss, remainder);
        v.push_back(remainder);
    }

    return v;
}

class ExplainProvenance {
protected:
    SouffleProgram& prog;

    std::vector<RamDomain> argsToNums(std::string relName, std::vector<std::string>& args) const {
        std::vector<RamDomain> nums;

        auto rel = prog.getRelation(relName);
        if (rel == nullptr) {
            return std::vector<RamDomain>();
        }

        for (size_t i = 0; i < args.size(); i++) {
            if (*rel->getAttrType(i) == 's') {
                nums.push_back(prog.getSymbolTable().lookupExisting(args[i].c_str()));
            } else {
                nums.push_back(std::stoi(args[i]));
            }
        }

        return nums;
    }

    std::vector<std::string> numsToArgs(const std::string relName, const std::vector<RamDomain>& nums,
            std::vector<bool>* err = nullptr) const {
        std::vector<std::string> args;

        auto rel = prog.getRelation(relName);
        if (rel == nullptr) {
            return args;
        }

        for (size_t i = 0; i < nums.size(); i++) {
            if (err && (*err)[i]) {
                args.push_back("_");
            } else {
                if (*rel->getAttrType(i) == 's') {
                    args.push_back(prog.getSymbolTable().resolve(nums[i]));
                } else {
                    args.push_back(std::to_string(nums[i]));
                }
            }
        }

        return args;
    }

public:
    ExplainProvenance(SouffleProgram& prog) : prog(prog) {}
    virtual ~ExplainProvenance() = default;

    virtual void setup() = 0;

    virtual std::unique_ptr<TreeNode> explain(
            std::string relName, std::vector<std::string> tuple, size_t depthLimit) = 0;

    virtual std::unique_ptr<TreeNode> explainSubproof(
            std::string relName, RamDomain label, size_t depthLimit) = 0;

    virtual std::string getRule(std::string relName, size_t ruleNum) = 0;
};

}  // end of namespace souffle
