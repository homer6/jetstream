#include "observability/prometheus/Metric.h"


namespace jetstream::observability::prometheus{

    Metric::Metric( const string& name, const string& help_message, const string& type )
        :name(name), help_message(help_message), type(type)
    {

    }


    const string& Metric::getName() const{
        return this->name;
    }


    void Metric::increment( const map<string,string>& labels, double amount ){

        this->increment( this->serializeLabels(labels), amount );

    }

    void Metric::increment( const string& labels, double amount ){

        if( this->counters.count(labels) ){
            this->counters.at(labels) += amount;
        }else{
            this->counters[labels] = amount;
        }

    }


    string Metric::serializeLabels( const map<string,string>& labels ) const{

        string result;
        const size_t labels_size = labels.size();
        size_t current_index = 0;

        for( const auto& [key, value] : labels ){
            result += key;
            result += "=\"";
            result += value;
            result += '"';
            if( current_index < labels_size - 1 ){
                result += ",";
            }
            current_index++;
        }

        return result;

    }



    ostream& operator<<( ostream& os, const Metric& metric ){

        if( metric.counters.size() ){

            os << "# HELP " << metric.name << " " << metric.help_message << "\n";
            os << "# TYPE " << metric.name << " " << metric.type << "\n";

            //3.16643328e+08

            for( auto& [labels, value] : metric.counters ){
                os << metric.name << "{" << labels << "} ";
                os << std::scientific << value << "\n";
            }

        }

        return os;

    }


}