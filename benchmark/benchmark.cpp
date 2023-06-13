/**
 * Copyright 2023, Leon Freist (https://github.com/lfreist)
 * Author: Leon Freist <freist.leon@gmail.com>
 * 
 * This file is part of x-search.
 */

#include <nanobench.h>
#include <aho-corasick-cjgdev.hpp>
#include <ac/nfa/nfa.h>

#include <iostream>
#include <cstring>
#include <fstream>

size_t ac_nfa (std::string &text, std::vector<std::string> &patterns)
{
        NFA nfa (patterns, MatchKind::STANDARD, false);
        State *state = nfa._start_state;
        size_t result = 0;
        for (auto c : text)
                {
                        if (state->is_match ())
                                {
                                        result += state->get_matches().size();
                                }
                        state = state->next_state (c);
                        if (state == nfa._dead_state)
                                {
                                        std::cout << "\n\ndead\n\n";
                                        break;
                                }
                }
        return result;
}

size_t ac_cjgdev (std::string &text, std::vector<std::string> &patterns)
{
        aho_corasick::trie t;
        for (auto &p : patterns)
                {
                        t.insert (p);
                }
        return t.parse_text (text).size ();
}

size_t naiv (std::string &text, std::vector<std::string> &patterns)
{
        size_t result = 0;

        for (const auto &p : patterns)
                {
                        const char *textPtr = text.c_str ();
                        const char *patternPtr = p.c_str ();

                        while ((textPtr = std::strstr (textPtr, patternPtr)) != nullptr)
                                {
                                        result++;
                                        textPtr += p.length ();
                                }
                }

        return result;
}

int main (int argc, char **argv)
{
        std::ifstream ifs ("files/harry_potter_1.txt");
        std::string text ((std::istreambuf_iterator<char> (ifs)),
                          (std::istreambuf_iterator<char> ()));

        std::vector<std::string> patterns = {
                "Hogwarts", "magical", "world", "Harry", "Potter", "wizard", "journey", "School", "Witchcraft",
                "Wizardry", "Ron", "Weasley", "Hermione", "Granger", "adventures", "challenges", "creatures",
                "Quidditch", "unraveled", "mysteries", "Philosopher's", "Stone", "friends", "courage", "triumphed",
                "Lord", "Voldemort", "dark", "killed", "parents", "wizarding", "spells", "broomstick", "enchanted",
                "sorting", "hat", "potion", "Snape", "Professor", "Dumbledore", "transfiguration", "magician", "spell",
                "quizzes", "fluffy", "Hagrid", "Quirrell", "ghosts", "chamber", "secrets", "Norbert", "Phoenix",
                "winged", "keys", "chess", "devil's", "mirror", "Sorcerer's", "castle", "Dobby", "elf", "goblin",
                "wand", "wizards", "witches", "cauldron", "Diagon", "Alley", "Weasleys", "Gringotts", "treasure",
                "Troll", "transformation", "transports", "flying", "invisibility", "cloak", "Muggle", "platform",
                "nine", "three-quarters", "spellbook", "spider", "aragog", "quill", "owl", "Azkaban", "dragon",
                "Triwizard", "Tournament", "unforgivable", "curses", "werewolf", "patronus", "Hogsmeade", "marauder's",
                "map", "Pensieve", "Dementors", "Merlin", "parseltongue", "Snitch", "Gryffindor", "Ravenclaw",
                "Hufflepuff", "Slytherin", "crucio", "expelliarmus", "sectumsempra", "accio", "lumos", "Nox", "quaffle",
                "bludger", "snitch", "seeker", "Ollivander", "Petrificus", "Totalus", "Wingardium", "Leviosa",
                "Alohomora", "Deluminator", "Polyjuice", "Remembrall", "time", "turner", "bezoar", "Flourish", "Blotts",
                "Florean", "Fortescue", "Gellert", "Grindelwald", "Nicholas", "Flamel", "Rita", "Skeeter", "Viktor",
                "Krum", "Molly", "Ginny", "Arthur", "Lucius", "Malfoy", "Neville", "Luna", "Lovegood", "Ollivanders",
                "Zonko's", "Filch", "Borgin", "Burkes", "Fudge", "Pomfrey", "Percy", "Fred", "George", "Charlie",
                "Bill", "adorable", "adventurous", "aggressive", "agreeable", "alert", "amazing", "ambitious",
                "amusing", "ancient", "angry", "annoyed", "anxious", "arrogant", "ashamed", "attractive", "average",
                "awesome", "awful", "bad", "beautiful", "better", "big", "bitter", "black", "blue", "boiling", "bold",
                "bored", "boring", "brave", "bright", "brilliant", "busy", "calm", "careful", "careless", "cautious",
                "charming", "cheerful", "clean", "clear", "clever", "cloudy", "clumsy", "colorful", "comfortable",
                "concerned", "confused", "content", "cooperative", "courageous", "crazy", "creepy", "crowded", "cruel",
                "curious", "cute", "dangerous", "dark", "dead", "deep", "defiant", "delicious", "delightful",
                "depressed", "determined", "different", "difficult", "dirty", "disgusted", "distinct", "disturbed",
                "dizzy", "doubtful", "drab", "dull", "eager", "easy", "elated", "elegant", "embarrassed", "enchanting",
                "evil", "excited", "expensive", "exuberant", "fair", "faithful", "famous", "fancy", "fantastic", "fast",
                "fat", "fierce", "filthy", "fine", "flaky", "flat", "fluffy", "foolish", "frail", "frantic", "fresh",
                "friendly", "frightened", "funny", "furry", "gentle", "gifted", "gigantic", "glamorous", "gleaming",
                "glorious", "good", "gorgeous", "graceful", "greedy", "green", "grieving", "grotesque", "grumpy",
                "handsome", "happy", "hard", "harsh", "healthy", "heavy", "helpful", "helpless", "hilarious",
                "homeless", "horrible", "hungry", "hurt", "icy", "ideal", "immense", "impatient", "important",
                "impossible", "innocent", "inquisitive", "itchy", "jealous", "jittery", "jolly", "joyous", "juicy",
                "kind", "large", "late", "lazy", "light", "little", "lively", "lonely", "long", "loose", "loud",
                "lovely", "lucky", "mad", "magnificent", "mammoth", "many", "massive", "mellow", "melodic", "melted",
                "mighty", "miserable", "moody", "nasty", "naughty", "nervous", "nice", "noisy", "normal", "nutty",
                "obedient", "obnoxious", "odd", "old", "orange", "ordinary", "outrageous", "overjoyed", "painful",
                "pale", "perfect", "pink", "plain", "pleasant", "poised", "poor", "powerful", "precious", "pretty",
                "prickly", "proud", "puzzled", "quaint", "quick", "quiet", "rainy", "rapid", "rare", "raspy", "red",
                "relieved", "repulsive", "rich", "ripe", "roasted", "robust", "rotten", "rough", "round", "salty",
                "scary", "scattered", "scrawny", "selfish", "shiny", "short", "shy", "silky", "silly", "skinny",
                "smiling", "smooth", "soft", "sore", "sour", "spicy", "splendid", "spotted", "square", "squeaky",
                "stale", "steady", "steep", "sticky", "stormy", "stout", "straight", "strange", "strong", "stupid",
                "successful", "sweet", "swift", "tall", "tame", "tasty", "tender", "terrible", "thankful", "thick",
                "thin", "thoughtful", "thundering", "tiny", "tired", "tough", "troubled", "ugliest", "ugly", "uneven",
                "uninterested", "unsightly", "unusual", "upset", "uptight", "vast", "victorious", "vivacious",
                "wandering", "warm", "weak", "weary", "wet", "whispering", "wide", "wild", "witty", "wonderful",
                "worried", "wretched", "yellow", "young", "yummy", "zany", "zealous", "encouraging", "energetic",
                "enthusiastic", "envious", "sleepy", "slimy", "slippery", "slow", "small", "smart",
        };

        std::cout << ac_cjgdev (text, patterns) << '\n';
        std::cout << naiv (text, patterns) << '\n';
        std::cout << ac_nfa (text, patterns) << std::endl;

        ankerl::nanobench::Bench bench;
        bench.title ("Aho-Corasick Comparisons")
                .unit ("byte")
                .batch (text.size ())
                .relative (true);

        auto add_benchmark = [&bench]<typename Op> (const std::string &name,
                                                    Op &&op) -> void
        {
          bench.run (name, std::forward<Op> (op));
        };

        add_benchmark ("cjgdev/aho-corasick", [&patterns, &text] ()
        {
          auto res = ac_cjgdev (text, patterns);
          ankerl::nanobench::doNotOptimizeAway (res);
        });
        add_benchmark ("std::strstr", [&patterns, &text] ()
        {
          auto res = naiv (text, patterns);
          ankerl::nanobench::doNotOptimizeAway (res);
        });
        add_benchmark ("lfreist/aho-corasick (NFA)", [&patterns, &text] ()
        {
          auto res = ac_nfa (text, patterns);
          ankerl::nanobench::doNotOptimizeAway (res);
        });

        return 0;
}