#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

/**
 * will return TRUE if the image is a CLR
 * FALSE otherwise
 */
static int read_pe64_data(FILE*, size_t);

int main(int argc, char** argv)
{
  uint32_t peHeader;
  uint16_t dataDictStart;
  uint32_t dataDictRVA[16];
  uint32_t dataDictSize[16];

  uint16_t peMagic;
  uint64_t dataDict64[16];

  struct PE_IMAGE_HEADER
  {
    uint32_t signature;
    uint16_t machine;
    uint16_t sections;
    uint32_t Timestamp;
    uint32_t SymbolTableOffset;
    uint32_t NumberOfSymbols;
    uint16_t OptionalHeaderSize;
    uint16_t Characteristics;
  } imageHeader;

  FILE* pf = NULL;
  if (argc < 2)
    {
      fprintf(stderr, "usage: %s [exe_path]\n", argv[0]);
      return 0;
    }

  if ((pf = fopen(argv[1], "r")) == NULL)
    {
      fprintf(stderr, "Unable to open %s for reading\n", argv[1]);
      return -1;
    }

  if (!fread(&peMagic, 1, 2, pf))
    {
      fprintf(stderr, "Could not read pe32_magic from file %s\n",
	      argv[1]);
      fclose(pf);
      return -1;
    }

  if (peMagic != 0x5A4D)
    {
      fprintf(stderr, "%s is not a PE32 executable.\n", argv[1]);
      fclose(pf);
      return 1;
    }


  /* PE Header Offset is at 0x3C, 4 bytes long */
  fseek(pf, 0x3C, SEEK_SET);
  if (fread(&peHeader, 1, 4, pf) < 4)
    {
      fprintf(stderr, "Unexpected EOF while reading PE Header\n");
      goto exit_and_close;
    }

  fseek(pf, peHeader, SEEK_SET);
  fread(&imageHeader, 1, sizeof(imageHeader), pf);

  if (imageHeader.machine == 0x014C)
    {
      /**
       * It should be defined outside main()
       * but it's now working properly below.
       *
       * This image is a i386 PE
       * printf("i386 PE Image\n");
       */
      goto read_i386_pe;
    }
  else if (imageHeader.machine == 0x8664)
    {
      /* comment out all printf()
       * printf("x86_64 PE Image\nOptional Header Size = %i\n", imageHeader.OptionalHeaderSize);
       */

      int nExec = read_pe64_data(pf, ftell(pf));
      if (nExec > 0)
	{
	  argv[0] = "mono";
	  int res = execvp("/usr/bin/mono", (char * const*)argv);
	  fprintf(stderr, "execvp() returns %i\n", res);
	  return res;
	}
      else if (nExec == 0)
	{
	  argv[0] = "wine64";
	  int res = execvp("/opt/wine/bin/wine64", (char* const*)argv);
	  fprintf(stderr, "error: execvp %i\n", res);
	  return res;
	}
      else
	{
	  // silently ignored
	  return -1;
	}
    }
  else if (imageHeader.machine == 0x0200)
    {
      printf("Most unlikely: Itanium PE Image\n");
      fclose(pf);
      return -1;
    }
  else
    {
      fclose(pf);
      printf("Unknown PE Machine type: 0x%X\n", imageHeader.machine);
      return -1;
    }

 read_i386_pe:
  if (!imageHeader.OptionalHeaderSize)
    {
      printf("Not a valid PE32 Executable\n");
      goto exit_and_close;
    }

  fseek(pf, 0x60, SEEK_CUR);
  dataDictStart = (uint16_t)(ftell(pf) & 0xFFFF);

  fread(dataDict64, 16, 8, pf);

  if (dataDict64[14] & 0xFFFFFFFF)
    {
      uint32_t rva = (uint32_t)(dataDict64[14] & 0xffffffff);
      uint32_t nSize = (uint32_t)(dataDict64[14] >> 32);

      fclose(pf);
      argv[0] = "mono";
      execvp("/usr/bin/mono", (char* const*)argv);
      fprintf(stderr, "error: execvp()\n");
      return -1;
    }
  else
    {
      fclose(pf);
      argv[0] = "wine";
      execvp("/opt/wine/bin/wine", (char* const*)argv);
      fprintf(stderr, "Error executing Wine32\n");
      return -1;
    }
  
exit_and_close:
  fclose(pf);
  return 0;
}

static int
read_pe64_data(FILE* pf, size_t offset)
{
  struct PE64_OPTIONAL_IMAGE_HEADER
  {
    uint16_t magic;
    uint8_t linkerVersionMajor;
    uint8_t linkerVersionMinor;
    uint32_t sizeOfCode;
    uint32_t sizeOfInitializedData;
    uint32_t sizeOfUninitializedData;
    uint32_t entryPointAddr;
    uint32_t baseAddr;
    uint64_t imageBase;
    uint32_t sectionAlignment;
    uint32_t fileAlignment;
    uint16_t OSMajor;
    uint16_t OSMinor;
    uint16_t imageMajor;
    uint16_t imageMinor;
    uint16_t subsystemMajor;
    uint16_t subsystemMinor;
    uint32_t Win32VersionValue;
    uint32_t imageSize;
    uint32_t headersSize;
    uint32_t checksum;
    uint16_t subsystem;
    uint16_t DllCharacteristics;
    uint64_t reservedData64[4];
    uint32_t dwLoaderFlags;
    uint32_t dwNumRVAAndSizes;
  } header;
  struct DataDirectory
  {
    uint32_t RVA;
    uint32_t Size;
  } dataDirectory[16];

  size_t nreads = 0;
  uint16_t magic = 0;

  /**
   * Directly go into RVA Data structures
   */
  fseek(pf, sizeof(header), SEEK_CUR);

  nreads = fread(dataDirectory, sizeof(dataDirectory[0]), 16, pf);
  if (dataDirectory[0].Size > 0 || dataDirectory[14].RVA > 0)
    {
      // If the image is PE32+ CLR then dataDirectory[0].Size could be 0,
      // but when we check dataDirectory[14].RVA it's always non-zero.
      fclose(pf);
      return 1;
    }

  // The image is not a CLR/CIL executable
  // But is a valid PE32+ executable
  fclose(pf);
  return 0;
}
