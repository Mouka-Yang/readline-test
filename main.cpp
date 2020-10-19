#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>

void print_completions(std::vector<std::string> completions){
    std::copy(completions.cbegin(), completions.cend(), std::ostream_iterator<std::string>(std::cout, "\n"));
    std::cout << std::endl;
}

class Option {
public:
    explicit Option(std::string name) : name_(std::move(name)) {}
    std::string getName() const {return name_;}
    std::vector<std::string> getCompletions() const {
        auto res = candidates_;
        res.push_back(name_);
        return res;
    }

    void addCandidate(const std::string& candidate) { candidates_.push_back(candidate); }
    void addCandidate(const std::vector<std::string>& candidates) { candidates_.insert(candidates_.end(), candidates.begin(), candidates.end()); }

private:
    std::string name_;
    std::vector<std::string> candidates_{};
};

class Command {
public:
    explicit Command(std::string name) : name_(std::move(name)) {}

    void addSubcommand(const Command& subcommand){ subcommands_.push_back(subcommand); }
    void addOption(const Option& option) { options_.push_back(option); }
    std::string getName() const { return name_; }

    std::vector<std::string> getCompletions() const {
        auto subcommandCompletions = getSubcommandCompletions();
        auto optionCompletions = getOptionCompletions();
        auto res = std::vector<std::string>{};
        res.reserve(subcommandCompletions.size() + optionCompletions.size());
        std::move(subcommandCompletions.begin(),subcommandCompletions.end(), std::back_inserter(res));
        std::move(optionCompletions.begin(),optionCompletions.end(), std::back_inserter(res));

        return res;
    }

    void my_print_completions() const {
        std::cout << "completions for " << getName() << "\n";
        print_completions(getCompletions());
        std::for_each(subcommands_.cbegin(), subcommands_.cend(), [](const auto& cmd) { cmd.my_print_completions(); });
    }

private:

    std::vector<std::string> getOptionCompletions() const {
        auto res = std::vector<std::string>{options_.size()};
        // std::transform(options_.cbegin(), options_.cend(), res.begin(), [](const auto& option){
        //         return option.getCompletions();
        //         });
        std::for_each(options_.cbegin(), options_.cend(), [&res](const auto& option){
            auto optionCompletions = option.getCompletions();
            res.insert(res.end(), optionCompletions.cbegin(), optionCompletions.cend());
            });

        return res;
    }

    std::vector<std::string> getSubcommandCompletions() const {
        auto res = std::vector<std::string>{subcommands_.size()};
        std::transform(subcommands_.cbegin(), subcommands_.cend(), res.begin(), [](const auto& subcommand){
                return subcommand.getName();
                });
        return res;
    }
    std::vector<Command> subcommands_{};
    std::vector<Option> options_{};
    std::string name_;

};


void print_fit(){
    Command app{"app"};
    Command cmdRegistry{"registry"};
    Command cmdEngine{"engine"};
    Command regShow{"show"};
    Command engShow{"show"};
    Command showFitables{"fitables"};
    Command showWorkers{"workers"};
    Command showSubscriptions{"subscriptions"};
    Command engShowFitables{"fitables"};
    Command engShowPlugins{"plugins"};
    
    Option ipFilter{"ip"};
    ipFilter.addCandidate(std::vector<std::string>{"--ip","-P"});
    showFitables.addOption(ipFilter);

    Option locationFilter{"location"};
    locationFilter.addCandidate(std::vector<std::string>{"--location","-L"});
    engShowPlugins.addOption(locationFilter);

    regShow.addSubcommand(showFitables);
    regShow.addSubcommand(showWorkers);
    regShow.addSubcommand(showSubscriptions);

    engShow.addSubcommand(engShowPlugins);
    engShow.addSubcommand(engShowFitables);

    cmdRegistry.addSubcommand(regShow);
    cmdEngine.addSubcommand(engShow);

    app.addSubcommand(cmdRegistry);
    app.addSubcommand(cmdEngine);

    app.my_print_completions();
}

int main(){
    Command subcommandOne{"sub1"};
    subcommandOne.addOption(Option{"op_sub1_1"});
    subcommandOne.addOption(Option{"op_sub1_2"});

    Command subcommandTwo{"sub2"};
    subcommandTwo.addOption(Option{"op_sub2_1"});

    Command app{"app"};
    app.addOption(Option{"option1"});
    app.addSubcommand(subcommandOne);
    app.addSubcommand(subcommandTwo);

    auto res = app.getCompletions();
    std::cout << "app command\n";
    print_completions(res);

    std::cout << "sub1 command\n";
    print_completions( subcommandOne.getCompletions());

    std::cout << "sub2 command\n";
    print_completions( subcommandTwo.getCompletions());

    print_fit();

}





