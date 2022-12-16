//     *************************** LiveVisionKit ****************************
//     Copyright (C) 2022  Sebastian Di Marco (crowsinc.dev@gmail.com)
//
//     This program is free software: you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.
//
//     This program is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
//
//     You should have received a copy of the GNU General Public License
//     along with this program.  If not, see <https://www.gnu.org/licenses/>.
//     **********************************************************************

#include <type_traits>

#include <iostream>
#include "FilterParser.hpp"


namespace clt
{
//---------------------------------------------------------------------------------------------------------------------

    std::shared_ptr<lvk::VideoFilter> FilterParser::try_parse(std::deque<std::string>& args)
    {
        if(OptionsParser::try_parse(args))
        {
            // m_ParsedConstructor is set when the options parsers finds a filter.
            auto filter = m_ParsedConstructor();
            filter.configure(args);

            return filter.instance;
        }
        return nullptr;
    }

//---------------------------------------------------------------------------------------------------------------------

    template<typename F, typename C>
    void FilterParser::generate_filter_constructor(
        const std::function<void(OptionsParser&, C&)>& config_connector
    )
    {
        m_ParsedConstructor = [&](){
            ConfigurableFilter filter;
            filter.instance = std::make_shared<F>();
            filter.configure = [&](std::deque<std::string>& args) {

                C filter_config;
                OptionsParser config_parser;
                config_parser.set_error_handler(m_ErrorHandler);

                config_connector(config_parser, filter_config);

                while(config_parser.try_parse(args));

                std::static_pointer_cast<lvk::Configurable<C>>(
                    std::static_pointer_cast<F>(filter.instance)
                )->configure(filter_config);
            };
            return filter;
        };
    }

//---------------------------------------------------------------------------------------------------------------------

    template<typename F, typename C>
    void FilterParser::add_filter(
        const std::initializer_list<std::string>& aliases,
        const std::string& description,
        const std::function<void(OptionsParser&, C&)>& config_connector
    )
    {
        static_assert(std::is_base_of_v<lvk::VideoFilter, F>, "Filter is not a derivation of VideoFilter");
        static_assert(std::is_base_of_v<lvk::Configurable<C>, F>, "Filter is not a derivation of Configurable<C>");

        generate_config_manual(aliases, config_connector);

        OptionsParser::add_switch(
            aliases,
            description,
            [=](){generate_filter_constructor<F, C>(config_connector);}
        );
    }

//---------------------------------------------------------------------------------------------------------------------

    template<typename F, typename C>
    void FilterParser::add_filter(
        const std::string& name,
        const std::string& description,
        const std::function<void(OptionsParser&, C&)>& config_connector
    )
    {
        add_filter({name}, description, config_connector);
    }

//---------------------------------------------------------------------------------------------------------------------

    bool FilterParser::has_filter(const std::string& alias) const
    {
        return m_FilterConstructors.contains(alias);
    }

//---------------------------------------------------------------------------------------------------------------------

    template<typename C>
    void FilterParser::generate_config_manual(
        const std::initializer_list<std::string>& aliases,
        const std::function<void(OptionsParser&, C&)>& config_connector
    )
    {
        // Create temporary config and option parser so we can grab the parser's manual
        C temp_config = {};
        OptionsParser config_parser;

        config_connector(config_parser, temp_config);

        const size_t index = m_ConfigManuals.size();
        m_ConfigManuals.push_back(config_parser.manual());
        for(const auto& name : aliases)
            m_ManualLookup[name] = index;
    }

//---------------------------------------------------------------------------------------------------------------------

    const std::string& FilterParser::config_manual(const std::string& filter) const
    {
        LVK_ASSERT(m_ManualLookup.contains(filter));

        return m_ConfigManuals[m_ManualLookup.at(filter)];
    }

//---------------------------------------------------------------------------------------------------------------------

    std::string FilterParser::manual(const std::string& filter) const
    {
        return OptionsParser::manual(filter);
    }

//---------------------------------------------------------------------------------------------------------------------

    const std::string& FilterParser::manual() const
    {
        return OptionsParser::manual();
    }

//---------------------------------------------------------------------------------------------------------------------

    void FilterParser::set_error_handler(const ErrorHandler& handler)
    {
        LVK_ASSERT(handler);

        m_ErrorHandler = handler;
    }

//---------------------------------------------------------------------------------------------------------------------
}
