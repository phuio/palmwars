typedef struct
{
    UInt8 x;
    UInt8 y;
    char  linkedStars[MAX_LINKS + 1];
} PWSTARPOS;

typedef struct
{
    UInt8     numStars;
    UInt8     reserved;
    PWSTARPOS s[1];
} PWSYSTEMDATA;

typedef struct
{
    UInt8 numSystems;
    UInt8 reserved;
} PWMAPHEADER;
