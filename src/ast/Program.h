/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file Program.h
 *
 * Defines the program class
 *
 ***********************************************************************/

#pragma once

#include "ast/Clause.h"
#include "ast/Component.h"
#include "ast/ComponentInit.h"
#include "ast/Directive.h"
#include "ast/FunctorDeclaration.h"
#include "ast/Node.h"
#include "ast/Pragma.h"
#include "ast/QualifiedName.h"
#include "ast/Relation.h"
#include "ast/Type.h"
#include "ast/utility/NodeMapper.h"
#include "souffle/utility/ContainerUtil.h"
#include "souffle/utility/StreamUtil.h"
#include <algorithm>
#include <cassert>
#include <iosfwd>
#include <memory>
#include <ostream>
#include <utility>
#include <vector>

namespace souffle {
class ParserDriver;
}
namespace souffle::ast {

namespace transform {
class ComponentInstantiationTransformer;
}
/**
 * @class Program
 * @brief The program class consists of relations, clauses and types.
 */
class Program : public Node {
public:
    /** Return types */
    std::vector<Type*> getTypes() const {
        return toPtrVector(types);
    }

    /** Return relations */
    std::vector<Relation*> getRelations() const {
        return toPtrVector(relations);
    }

    /** Return clauses */
    std::vector<Clause*> getClauses() const {
        return toPtrVector(clauses);
    }

    /** Return functor declarations */
    std::vector<FunctorDeclaration*> getFunctorDeclarations() const {
        return toPtrVector(functors);
    }

    /** Return relation directives */
    std::vector<Directive*> getDirectives() const {
        return toPtrVector(directives);
    }

    /** Add relation directive */
    void addDirective(Own<Directive> directive) {
        assert(directive && "NULL directive");
        directives.push_back(std::move(directive));
    }

    /** Return pragma directives */
    const VecOwn<Pragma>& getPragmaDirectives() const {
        return pragmas;
    }

    /* Add relation */
    void addRelation(Own<Relation> relation);

    /** Remove relation */
    bool removeRelationDecl(const QualifiedName& name);

    /** Set clauses */
    void setClauses(VecOwn<Clause> newClauses) {
        clauses = std::move(newClauses);
    }

    /** Add a clause */
    void addClause(Own<Clause> clause);

    /** Add a type declaration */
    void addType(Own<Type> type);

    /** Remove a clause */
    bool removeClause(const Clause* clause);

    /** Remove a directive */
    bool removeDirective(const Directive* directive);

    /** Return components */
    std::vector<Component*> getComponents() const {
        return toPtrVector(components);
    }

    /** Return component instantiation */
    std::vector<ComponentInit*> getComponentInstantiations() const {
        return toPtrVector(instantiations);
    }

    /** Remove components and components' instantiations */
    void clearComponents();

    Program* clone() const override {
        auto res = new Program();
        res->pragmas = souffle::clone(pragmas);
        res->components = souffle::clone(components);
        res->instantiations = souffle::clone(instantiations);
        res->types = souffle::clone(types);
        res->functors = souffle::clone(functors);
        res->relations = souffle::clone(relations);
        res->clauses = souffle::clone(clauses);
        res->directives = souffle::clone(directives);
        return res;
    }

    void apply(const NodeMapper& map) override {
        for (auto& cur : pragmas) {
            cur = map(std::move(cur));
        }
        for (auto& cur : components) {
            cur = map(std::move(cur));
        }
        for (auto& cur : instantiations) {
            cur = map(std::move(cur));
        }
        for (auto& cur : functors) {
            cur = map(std::move(cur));
        }
        for (auto& cur : types) {
            cur = map(std::move(cur));
        }
        for (auto& cur : relations) {
            cur = map(std::move(cur));
        }
        for (auto& cur : clauses) {
            cur = map(std::move(cur));
        }
        for (auto& cur : directives) {
            cur = map(std::move(cur));
        }
    }

    ChildNodes getChildNodes() const override {
        ChildNodes res;
        append(res, makeDerefRange(pragmas));
        append(res, makeDerefRange(components));
        append(res, makeDerefRange(instantiations));
        append(res, makeDerefRange(functors));
        append(res, makeDerefRange(types));
        append(res, makeDerefRange(relations));
        append(res, makeDerefRange(clauses));
        append(res, makeDerefRange(directives));
        return res;
    }

protected:
    void print(std::ostream& os) const override {
        auto show = [&](auto&& xs, char const* sep = "\n") {
            if (!xs.empty()) os << join(xs, sep) << "\n";
        };

        show(pragmas, "\n\n");
        show(components);
        show(instantiations);
        show(types);
        show(functors);
        show(relations);
        show(clauses, "\n\n");
        show(directives, "\n\n");
    }

    bool equal(const Node& node) const override {
        const auto& other = static_cast<const Program&>(node);
        if (!equal_targets(pragmas, other.pragmas)) {
            return false;
        }
        if (!equal_targets(components, other.components)) {
            return false;
        }
        if (!equal_targets(instantiations, other.instantiations)) {
            return false;
        }
        if (!equal_targets(functors, other.functors)) {
            return false;
        }
        if (!equal_targets(types, other.types)) {
            return false;
        }
        if (!equal_targets(relations, other.relations)) {
            return false;
        }
        if (!equal_targets(clauses, other.clauses)) {
            return false;
        }
        if (!equal_targets(directives, other.directives)) {
            return false;
        }
        return true;
    }

protected:
    friend class souffle::ParserDriver;

    void addPragma(Own<Pragma> pragma);

    void addFunctorDeclaration(Own<FunctorDeclaration> functor);

    /** Add component */
    void addComponent(Own<Component> component) {
        assert(component && "NULL component");
        components.push_back(std::move(component));
    }

    /** Add component instantiation */
    void addInstantiation(Own<ComponentInit> instantiation) {
        assert(instantiation && "NULL instantiation");
        instantiations.push_back(std::move(instantiation));
    }

    /** Program types  */
    VecOwn<Type> types;

    /** Program relations */
    VecOwn<Relation> relations;

    /** External Functors */
    VecOwn<FunctorDeclaration> functors;

    /** Program clauses */
    VecOwn<Clause> clauses;

    /** Directives */
    VecOwn<Directive> directives;

    /** Component definitions */
    VecOwn<Component> components;

    /** Component instantiations */
    VecOwn<ComponentInit> instantiations;

    /** Pragmas */
    VecOwn<Pragma> pragmas;
};

}  // namespace souffle::ast
