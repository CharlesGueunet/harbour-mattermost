#ifndef CPPHASH_H
#define CPPHASH_H

class CppHashPrivate;
class CppHash
{
public:
    CppHash();

    static CppHash* instance();

    int find_emoji(const char *emoji_short_name, int size, char **picture_path, int *header);

    int count() const;
private:
    CppHashPrivate *d;
};

#endif // CPPHASH_H
