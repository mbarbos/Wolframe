**
**requires:CJSON
**requires:TEXTWOLF
**input
{
  "doctype" : "CDCatalog",
  "CATALOG": {
    "CD": [
      {
        "TITLE": "Empire Burlesque",
        "ARTIST": "Bob Dylan",
        "RELEASE": {
          "COUNTRY": "USA",
          "COMPANY": "Columbia",
          "PRICE": "10.90",
          "YEAR": "1985"
        }
      },
      {
        "TITLE": "El Rei el Cantinero",
        "ARTIST": "Jose Alfredo Jimenez",
        "RELEASE": {
          "COUNTRY": "Mexico",
          "COMPANY": "RCA",
          "PRICE": "15.90",
          "YEAR": "1958"
        }
      },
      {
        "TITLE": "Still got the blues",
        "ARTIST": "Gary Moore",
        "RELEASE": {
          "COUNTRY": "UK",
          "COMPANY": "Virgin records",
          "PRICE": "10.20",
          "YEAR": "1990"
        }
      },
      {
        "TITLE": "Eros",
        "ARTIST": "Eros Ramazzotti",
        "RELEASE": {
          "COUNTRY": "EU",
          "COMPANY": "BMG",
          "PRICE": "9.90",
          "YEAR": "1997"
        }
      },
      {
        "TITLE": "One night only",
        "ARTIST": "Bee Gees",
        "RELEASE": {
          "COUNTRY": "UK",
          "COMPANY": "Polydor",
          "PRICE": "10.90",
          "YEAR": "1998"
        }
      },
      {
        "TITLE": "Sylvias Mother",
        "ARTIST": "Dr.Hook",
        "RELEASE": {
          "COUNTRY": "UK",
          "COMPANY": "CBS",
          "PRICE": "8.10",
          "YEAR": "1973"
        }
      },
      {
        "TITLE": "Romanza",
        "ARTIST": "Andrea Bocelli",
        "RELEASE": {
          "COUNTRY": "EU",
          "COMPANY": "Polydor",
          "PRICE": "10.80",
          "YEAR": "1996"
        }
      }
    ]
  }
}**config
--input-filter cjson --output-filter cjson --module ../../src/modules/filter/cjson/mod_filter_cjson  --module ../../src/modules/cmdbind/directmap/mod_command_directmap --module ../../src/modules/normalize/number/mod_normalize_number --module ../../src/modules/normalize/string/mod_normalize_string --module ../../src/modules/ddlcompiler/simpleform/mod_ddlcompiler_simpleform --module ../wolfilter/modules/cmdbind/mylang/mod_command_mylang --cmdprogram=example.dmap --program=example.sfrm --program=example.wnmp --program=example.mlg getCDCatalog

**file:example.sfrm
FORM CDCatalog
{
	CATALOG
	{
		CD []
		{
			TITLE string
			ARTIST string
			RELEASE
			{
				COUNTRY string
				COMPANY string
				PRICE string
				YEAR int
			}
		}
	}
}
**file:example.wnmp
int=number:integer;
uint=number:unsigned;
float=number:float;
normname=string:convdia,lcname;
**file:example.dmap
COMMAND(get CDCatalog) CALL(run) RETURN(CDCatalog);
**file:example.mlg
run
**output
{
	"doctype":	"CDCatalog.simpleform",
	"CATALOG":	{
		"CD":	[{
				"TITLE":	"EMPIRE BURLESQUE",
				"ARTIST":	"BOB DYLAN",
				"RELEASE":	{
					"COUNTRY":	"USA",
					"COMPANY":	"COLUMBIA",
					"PRICE":	"10.90",
					"YEAR":	"1985"
				}
			}, {
				"TITLE":	"EL REI EL CANTINERO",
				"ARTIST":	"JOSE ALFREDO JIMENEZ",
				"RELEASE":	{
					"COUNTRY":	"MEXICO",
					"COMPANY":	"RCA",
					"PRICE":	"15.90",
					"YEAR":	"1958"
				}
			}, {
				"TITLE":	"STILL GOT THE BLUES",
				"ARTIST":	"GARY MOORE",
				"RELEASE":	{
					"COUNTRY":	"UK",
					"COMPANY":	"VIRGIN RECORDS",
					"PRICE":	"10.20",
					"YEAR":	"1990"
				}
			}, {
				"TITLE":	"EROS",
				"ARTIST":	"EROS RAMAZZOTTI",
				"RELEASE":	{
					"COUNTRY":	"EU",
					"COMPANY":	"BMG",
					"PRICE":	"9.90",
					"YEAR":	"1997"
				}
			}, {
				"TITLE":	"ONE NIGHT ONLY",
				"ARTIST":	"BEE GEES",
				"RELEASE":	{
					"COUNTRY":	"UK",
					"COMPANY":	"POLYDOR",
					"PRICE":	"10.90",
					"YEAR":	"1998"
				}
			}, {
				"TITLE":	"SYLVIAS MOTHER",
				"ARTIST":	"DR.HOOK",
				"RELEASE":	{
					"COUNTRY":	"UK",
					"COMPANY":	"CBS",
					"PRICE":	"8.10",
					"YEAR":	"1973"
				}
			}, {
				"TITLE":	"ROMANZA",
				"ARTIST":	"ANDREA BOCELLI",
				"RELEASE":	{
					"COUNTRY":	"EU",
					"COMPANY":	"POLYDOR",
					"PRICE":	"10.80",
					"YEAR":	"1996"
				}
			}]
	}
}
**end