#include <iostream>
#include <map>
#include <thread>
#include <memory>
#include <vector>

#include <boost/asio.hpp>
#include <boost/variant.hpp>
#include <boost/signals2.hpp>
#include <boost/optional/optional_io.hpp>


// Leave only one uncommented
//#define EXAMPLE_1_HELLO_WORLD
//#define EXAMPLE_2_HELLO_WORLD
//#define EXAMPLE_3_VALUES_TO_AND_FROM_SLOTS
//#define EXAMPLE_4_CONNECTION_MANAGEMENT
#define EXAMPLE_5_AUTOMATIC_CONNECTION_MANAGEMENT


#ifdef EXAMPLE_1_HELLO_WORLD

struct Hello
{
    void operator()() const
    {
        std::cout << "Hello";
    }
};
struct World
{
    void operator()() const
    {
        std::cout << ", World!" << std::endl;
    }
};
int main(int argc, char **argv)
{
    /* Signal with no arguments and a void return value */
    boost::signals2::signal<void()> sig;

    /* By default, slots are pushed onto the back of the slot list */
    sig.connect(Hello());
    sig.connect(World());

    /* In this case, the slots are evoked in the order in which they were connected*/
    sig();
}

#elif defined(EXAMPLE_2_HELLO_WORLD)

struct Hello
{
    void operator()() const
    {
        std::cout << "Hello";
    }
};
struct World
{
    void operator()() const
    {
        std::cout << ", World!" << std::endl;
    }
};
struct GoodMorning
{
    void operator()() const
    {
        std::cout << "... and good morning!" << std::endl;
    }
};
struct FirstOfAll
{
    void operator()() const
    {
        std::cout << "First of all..." << std::endl;
    }
};
int main(int argc, char **argv)
{
    /* Signal with no arguments and a void return value */
    boost::signals2::signal<void()> sig;

    /* If we want to specify order, we can supply an extra parameter at the beginning of the connect
     * call that specifies the group. Group values are, by default, ints, and are ordered by the
     * integer < relation, group 0 precedes group 1*/

    /* Connect slot to signal with group 1 */
    sig.connect(1, World());
    /* Connect slot to signal with group 0 */
    sig.connect(0, Hello());
    /* By default slots are connected at the end of the slot list */
    sig.connect(GoodMorning());
    /* Can specify special last parameter to connect to place slot in front */
    sig.connect(FirstOfAll(), boost::signals2::at_front);

    sig();
}

#elif defined(EXAMPLE_3_VALUES_TO_AND_FROM_SLOTS)

void print_args(float x, float y)
{
    std::cout << "The arguments are " << x << " and " << y << std::endl;
}

void print_sum(float x, float y)
{
    std::cout << "The sum is " << x + y << std::endl;
}
float product(float x, float y)
{
    return x * y;
}
float quotient(float x, float y)
{
    return x / y;
}
/* This is a combiner which places all the values returned from slots into a container */
template <typename Container>
struct aggregate_values
{
    typedef Container result_type;

    template <typename InputIterator>
    Container operator()(InputIterator first, InputIterator last) const
    {
        Container values;

        while (first != last)
        {
            values.push_back(*first);
            ++first;
        }
        return values;
    }
};
int main(int argc, char **argv)
{
    /* Signal with 2 float arguments and a void return value */
    boost::signals2::signal<void(float, float)> signal1;

    signal1.connect(&print_args);
    signal1.connect(&print_sum);

    signal1(8.0, .5);

    /* Signal with 2 float arguments and a float return value */
    boost::signals2::signal<float(float, float)> signal2;

    signal2.connect(&product);
    signal2.connect(&quotient);

    /* [WARNING] This outputs only the quotient result (and not the product) */
    std::cout << "This is the non-aggregate output: " << signal2(10, 3.4) << std::endl;

    /* Signal with 2 float arguments and a float return value */
    boost::signals2::signal<float(float, float), aggregate_values<std::vector<float> > > signal3;

    signal3.connect(&product);
    signal3.connect(&quotient);

    /* Now this outputs the results from all slots according to the specified combiner */
    std::vector<float> results = signal3(25, 5);
    std::cout << "These are the aggregate values: ";
    std::copy(results.begin(), results.end(), std::ostream_iterator<float>(std::cout, " "));
    std::cout << "\n";
}

#elif defined(EXAMPLE_4_CONNECTION_MANAGEMENT)

struct HelloWorld
{
    mutable int m_ctr;

    HelloWorld() : m_ctr{0}
    {
    }

    void operator()() const
    {
        m_ctr++;
        std::cout << "Hello World " << m_ctr << std::endl;
    }
};
struct ShortLived
{
    void operator()() const
    {
        std::cout << "ShortLived" << std::endl;
    }
};

int main(int argc, char **argv)
{
    /* Signal with no arguments and a void return value */
    boost::signals2::signal<void()> signal1;

    /* Create a connection object and associate it with the connection between signal1 & the slot
     * HelloWorld() */
    boost::signals2::connection connection1 = signal1.connect(HelloWorld());

    // -> Blocks the slot
    // Prints OK...
    signal1();
    {
        // Block the slot
        boost::signals2::shared_connection_block block(connection1);
        // Prints nothing...
        signal1();
    }

    // Prints OK...
    signal1();

    // -> Disconnects the slot
    // Disconnects
    connection1.disconnect();
    // Prints nothing...
    signal1();

    {
        // Creates scoped connection
        boost::signals2::scoped_connection connection2(signal1.connect(ShortLived()));
        // Prints OK...
        signal1();
    }  // scoped_connection goes out of scope and disconnects

    // Prints nothing...
    signal1();
}

#elif defined(EXAMPLE_5_AUTOMATIC_CONNECTION_MANAGEMENT)

#include <boost/bind/bind.hpp>

class NewsItem
{
   public:
    NewsItem(const char* text) : m_text(text)
    {
    }
    const std::string& text() const
    {
        return m_text;
    }

   private:
    std::string m_text;
};

class GenericMessageArea
{
};

class NewsMessageArea : public GenericMessageArea
{
   public:
    NewsMessageArea()
    {
    }

    void update() const
    {
        std::cout << m_latest_news << std::endl;
    }

    void displayNews(const NewsItem& news) const
    {
        m_latest_news = news.text();
        update();
    }

   private:
    mutable std::string m_latest_news;
};

typedef boost::signals2::signal<void(const NewsItem&)> signal_type;
signal_type                                            signal_news_delivery;

int main(int argc, char** argv)
{
    /* A slot will automatically disconnect when any of its tracked objects expire. Also, no tracked
    object expires while the slot it is associated with is in mid-execution. To track a connection,
    use a shared_pointer and one of ".track" OR ".track_foreign" methods*/
    {
        std::shared_ptr<NewsMessageArea> messageAreaObject = std::make_shared<NewsMessageArea>();
        signal_news_delivery.connect(signal_type::slot_type(&NewsMessageArea::displayNews,
                                                            messageAreaObject.get(),
                                                            boost::placeholders::_1)
                                         .track_foreign(messageAreaObject));

        // Prints OK...
        signal_news_delivery(NewsItem("Great news!"));
    }

    // Prints nothing...
    signal_news_delivery(NewsItem("Great news!"));

    return 0;
}

#endif