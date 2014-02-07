#include "appDevel.hpp"

using namespace _Wolframe;

class ConversionResources
    :public types::NormalizeResourceHandle
{
public:
    ConversionResources()
    {}
    virtual ~ConversionResources()
    {}
};

class NormalizeInt
        :public types::NormalizeFunction
{
public:
    explicit NormalizeInt( const ConversionResources* res_)
        :res(res_){}
    virtual ~NormalizeInt()
        {}
    virtual const char* name() const
        {return "int";}
    virtual types::Variant execute( const types::Variant& i) const
        {return types::Variant( i.toint());}

    static types::NormalizeFunction* create(
        types::NormalizeResourceHandle* reshnd, const std::string&)
    {
        ConversionResources* res
            = dynamic_cast<ConversionResources*>(reshnd);
        return new NormalizeInt( res);
    }
private:
    const ConversionResources* res;
};

NORMALIZER_MODULE_WITH_RESOURCE( 
    "example2",
    "normalizer module with resources",
    ConversionResources)

NORMALIZER_FUNCTION( "Int", NormalizeInt::create)
NORMALIZER_MODULE_END


