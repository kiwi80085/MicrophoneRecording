#include <iostream>
#include <Windows.h>
#include <mmsystem.h>
#include <fstream>

#pragma comment(lib,"winmm.lib")
#pragma warning(disable:4996)
using namespace std;

#define Seconds_Of_Recording 10; // Enter the recording time in seconds

void Record()
{
    const int NUMPTS = 5513 * 4 * Seconds_Of_Recording;
    short int waveIn[NUMPTS];

    int sampleRate = 5513;

    const char* filename = "output.wav";

    WAVEFORMATEX pFormat;
    pFormat.wFormatTag = WAVE_FORMAT_PCM;     // simple, uncompressed format
    pFormat.nChannels = 2;                    //  1=mono, 2=stereo
    pFormat.wBitsPerSample = 16;              //  16 for high quality, 8 for telephone-grade
    pFormat.nSamplesPerSec = sampleRate;
    pFormat.nAvgBytesPerSec = sampleRate * pFormat.nChannels * pFormat.wBitsPerSample / 8;
    pFormat.nBlockAlign = pFormat.nChannels * pFormat.wBitsPerSample / 8;
    pFormat.cbSize = 0;

    HWAVEIN hWaveIn;
    WAVEHDR waveInHdr;

    waveInOpen(&hWaveIn, WAVE_MAPPER, &pFormat, 0L, 0L, WAVE_FORMAT_DIRECT);

    waveInHdr.lpData = (LPSTR)waveIn;
    waveInHdr.dwBufferLength = NUMPTS * 2;
    waveInHdr.dwBytesRecorded = 0;
    waveInHdr.dwUser = 0L;
    waveInHdr.dwFlags = 0L;
    waveInHdr.dwLoops = 0L;

    waveInPrepareHeader(hWaveIn, &waveInHdr, sizeof(WAVEHDR));

    waveInAddBuffer(hWaveIn, &waveInHdr, sizeof(WAVEHDR));

    waveInStart(hWaveIn);

    // Wait until finished recording
    do {} while (waveInUnprepareHeader(hWaveIn, &waveInHdr, sizeof(WAVEHDR)) == WAVERR_STILLPLAYING);

    waveInClose(hWaveIn);
    const DWORD dwFmtSize = 16;
    DWORD dwWaveSize = NUMPTS;
    const DWORD dwFileSize = dwWaveSize + 36; // Size of File

    FILE* fp = fopen(filename, "wb");
    if (fp == NULL) {
        std::cerr << "Failed to open " << filename << std::endl;
        return;
    }
    fwrite("RIFF", sizeof(BYTE), 4, fp);
    fwrite(&dwFileSize, sizeof(DWORD), 1, fp);
    fwrite("WAVE", sizeof(BYTE), 4, fp);

    fwrite("fmt ", sizeof(BYTE), 4, fp);
    fwrite(&dwFmtSize, sizeof(DWORD), 1, fp);
    fwrite(&pFormat, 1, 16, fp);
    fwrite("data", sizeof(BYTE), 4, fp);
    fwrite(&dwWaveSize, sizeof(DWORD), 1, fp);
    fwrite(waveIn, sizeof(BYTE), dwWaveSize, fp);
    fclose(fp);
}

void main()
{
    Record();
}