#pragma once

#include "tree.h"

namespace Matcher {

template< typename M >
bool match_one(const AstNode& node, M&& matcher);

template< typename M >
bool match_one(const Tree& tree, M&& matcher);

template< typename M, typename... Ms >
bool match_impl(const children_t& children, size_t n, M&& matcher, Ms&&... rest);

bool match_impl(const children_t& children, size_t n);

template< typename M, typename Ms >
struct WithChildren {
    M matcher;
    Ms child_matchers;

    WithChildren(M&& matcher, Ms&& matchers)
        : matcher(std::forward<M>(matcher))
        , child_matchers(std::forward<Ms>(child_matchers)) {}

    bool operator()(const Tree& tree) {
        if (!match_one(tree, matcher)) {
            return false;
        }

        return std::apply([&tree](auto&&... matchers) {
            return match_impl(tree.get_children(), 0, matchers...);
        }, child_matchers);
    }
};

inline bool any() {
    return true;
}

template< typename... Ms >
auto children(Ms&&... matchers) {
    return WithChildren(static_cast<bool(*)()>(any), std::tuple<Ms...>(std::forward<Ms>(matchers)...));
}

template< typename M, typename... Ms >
auto with_children(M&& matcher, Ms&&... child_matchers) {
    return WithChildren(std::forward<M>(matcher), std::tuple<Ms...>(std::forward<Ms>(child_matchers)...));
}

}
