
#include <mitie/text_feature_extraction.h>
#include <mitie/text_categorizer.h>

using namespace dlib;

namespace mitie
{

// ----------------------------------------------------------------------------------------

    text_categorizer::
    text_categorizer(
        const std::vector<std::string>& tag_name_strings_,
        const total_word_feature_extractor& fe_,
        const dlib::multiclass_linear_decision_function<dlib::sparse_linear_kernel<text_sample_type>,unsigned long>& df_
    ) : tag_name_strings(tag_name_strings_), fe(fe_), df(df_)
    {
        // make sure the requirements are not violated.
        DLIB_CASSERT(df.number_of_classes() >= tag_name_strings.size(),"invalid inputs");

        std::set<unsigned long> df_tags(df.get_labels().begin(), df.get_labels().end());
        for (unsigned long i = 0; i < tag_name_strings.size(); ++i)
        {
            DLIB_CASSERT(df_tags.count(i) == 1, "The classifier must be capable of predicting each possible tag as output.");
        }

        compute_fingerprint();
    }

// ----------------------------------------------------------------------------------------

    text_categorizer::
    text_categorizer(const std::string& pureModelName,
                     const std::string& extractorName
    ) {
        std::string classname;
        dlib::deserialize(pureModelName) >> classname;
        if (classname != "mitie::text_categorizer_pure_model")
            throw dlib::error(
                    "This file does not contain a mitie::text_categorizer_extractor_pure_model. Contained: " + classname);

        dlib::deserialize(pureModelName) >> classname >> df >> tag_name_strings;

        dlib::deserialize(extractorName) >> classname;
        if (classname != "mitie::total_word_feature_extractor")
            throw dlib::error(
                    "This file does not contain a mitie::total_word_feature_extractor. Contained: " + classname);

        dlib::deserialize(extractorName) >> classname >> fe;
    }
// ----------------------------------------------------------------------------------------

    text_categorizer::
    text_categorizer(const std::string& pureModelName)
    {
        std::string classname;
        dlib::deserialize(pureModelName) >> classname;
        if (classname != "mitie::text_categorizer_pure_model")
            throw dlib::error(
                    "This file does not contain a mitie::text_categorizer_extractor_pure_model. Contained: " + classname);

        dlib::deserialize(pureModelName) >> classname >> df >> tag_name_strings;
    }

// ----------------------------------------------------------------------------------------

    void text_categorizer::
    predict (
        const std::vector<std::string>& sentence,
        string& text_tag,
        double& text_score
    ) const {

        std::pair<unsigned long, double> temp;

        if (fe.get_num_dimensions() == 0) {
            temp = df.predict(extract_BoW_features(sentence));
        } else {
            const std::vector<matrix<float, 0, 1> > &sent = sentence_to_feats(fe, sentence);
            temp = df.predict(extract_combined_features(sentence, sent));
        }

        // now label the document
        unsigned long text_tag_id = temp.first;
        if(text_tag_id < tag_name_strings.size()) text_tag = tag_name_strings[text_tag_id];
        else text_tag = "Unseen";
        text_score = temp.second;
    }

// ----------------------------------------------------------------------------------------

    void text_categorizer::
    operator() (
        const std::vector<std::string>& sentence,
        string& text_tag
    ) const
    {
        std::pair<unsigned long, double> temp;

        if (fe.get_num_dimensions() == 0) {
            temp = df.predict(extract_BoW_features(sentence));
        } else {
            const std::vector<matrix<float, 0, 1> > &sent = sentence_to_feats(fe, sentence);
            temp = df.predict(extract_combined_features(sentence, sent));
        }

        // now label the document
        unsigned long text_tag_id = temp.first;
        if(text_tag_id < tag_name_strings.size()) text_tag = tag_name_strings[text_tag_id];
        else text_tag = "Unseen";
    }

// ----------------------------------------------------------------------------------------

}

