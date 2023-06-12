#include <ac/nfa/nfa.h>
#include <iostream>

int main (int argc, char **argv)
{
        std::vector<std::string> patterns {"pattern", "attr", "pater", "att", "patt"};
        NFA nfa (patterns, MatchKind::STANDARD, true);
        std::string test (
                "pattern PATTERN containing all from pater to attr to partial patterns."
        );
        auto *state = nfa._start_state;
        size_t offset = 0;
        for (auto c : test)
                {
                        if (state->is_match ())
                                {
                                        auto matches = state->get_matches();
                                        for (auto i : matches) {
                                                std::cout << *i << " (" << offset - i->size() << ")\n";
                                        }
                                }
                        state = state->next_state (c);
                        if (state == nfa._fail_state)
                                {
                                        state = nfa._start_state;
                                }
                        if (state == nfa._dead_state)
                                {
                                        std::cout << "\n\ndead\n\n";
                                }
                        ++offset;
                }
        return 0;
}