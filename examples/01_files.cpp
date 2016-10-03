#include <Kraken/IO/File.h>
#include <iostream>

using namespace std;
using namespace Kraken;

int UselessFunnel()
{
    cout << __PRETTY_FUNCTION__ << endl;

    uint8_t firstBuffer[1024] = { 0 };
    uint8_t secondBuffer[512] = { 0 };
    File in, out;

    int err = in.Open("/dev/urandom", EFileFlags::Read);
    if (err)
    {
        cerr << "Failed to open file, err = " << err << endl;
        return EXIT_FAILURE;
    }

    err = out.Open("/dev/null", EFileFlags::Write);
    if (err)
    {
        cerr << "Failed to open file, err = " << err << endl;
        return EXIT_FAILURE;
    }

    cout << "\t>> Res = " << in.Read(firstBuffer, sizeof(firstBuffer)) << endl; // Plain and simple: read N bytes into a buffer.
    cout << "\t>> Res = " << in.Read(secondBuffer) << endl; // The buffer is converted to a membuf by a template constructor.
    cout << "\t>> Res = " << out.Write(firstBuffer) << endl;
    cout << "\t>> Res = " << out.Write(secondBuffer, sizeof(secondBuffer)) << endl;

    return EXIT_SUCCESS;
}

#include <Kraken/Collections.h>
int Pipes()
{
    cout << __PRETTY_FUNCTION__ << endl;

    buffer<16> buf;
    File read, write;
    int err;

    err = File::Pipe(read, write);
    if (err)
    {
        cerr << "Failed to create pipe, err = " << err << endl;
        return EXIT_FAILURE;
    }

    cout << "\t>> Res = " << write.Write(buf) << endl;
    cout << "\t>> Res = " << read.Read(buf) << endl;

    return EXIT_SUCCESS;
}

int main()
{
    if (UselessFunnel())
        return EXIT_FAILURE;

    if (Pipes())
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}