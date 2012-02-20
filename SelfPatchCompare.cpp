/*=========================================================================
 *
 *  Copyright David Doria 2011 daviddoria@gmail.com
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include "SelfPatchCompare.h"

#include "Helpers.h"
#include "Patch.h"
#include "Types.h"

SelfPatchCompare::SelfPatchCompare(const unsigned int components)
{
  this->NumberOfComponentsPerPixel = components;
  
  this->NumberOfPixelsCompared = 0;
}

void SelfPatchCompare::ComputeSourcePatches()
{
  // Find all full patches that are entirely Valid
  
  std::cout << "ComputeSourcePatches() with patch size: " << this->TargetRegion.GetSize() << std::endl;
  
  this->SourcePatches.clear();
  itk::ImageRegionConstIterator<FloatVectorImageType> imageIterator(this->Image, this->Image->GetLargestPossibleRegion());

  while(!imageIterator.IsAtEnd())
    {
    itk::Index<2> currentPixel = imageIterator.GetIndex();
    itk::ImageRegion<2> region = Helpers::GetRegionInRadiusAroundPixel(currentPixel, this->TargetRegion.GetSize()[0]/2);
  
    if(this->MaskImage->GetLargestPossibleRegion().IsInside(region))
      {
      if(this->MaskImage->IsValid(region))
	{
	this->SourcePatches.push_back(Patch(region));
	//DebugMessage("Added a source patch.");
	}
      }
  
    ++imageIterator;
    }
  std::cout << "There are " << this->SourcePatches.size() << " source patches." << std::endl;
  
}

void SelfPatchCompare::SetNumberOfComponentsPerPixel(const unsigned int value)
{
  this->NumberOfComponentsPerPixel = value;
}

bool SelfPatchCompare::IsReady()
{
  if(this->Image && this->MaskImage && NumberOfComponentsPerPixel > 0)
    {
    return true;
    }
  else
    {
    return false;
    }
}

void SelfPatchCompare::SetImage(FloatVectorImageType::Pointer image)
{
  this->Image = image;
}

void SelfPatchCompare::SetMask(Mask::Pointer mask)
{
  this->MaskImage = mask;
}

void SelfPatchCompare::SetTargetRegion(const itk::ImageRegion<2>& region)
{
  this->TargetRegion = region;
}

float SelfPatchCompare::PixelSquaredDifference(const VectorType &a, const VectorType &b)
{
  float difference = 0;
  
  float diff = 0;
  for(unsigned int i = 0; i < this->NumberOfComponentsPerPixel; ++i)
    {
    diff = a[i] - b[i];
    difference += diff * diff;
    }
  return difference;
}


float SelfPatchCompare::PixelDifference(const VectorType &a, const VectorType &b)
{
  float difference = 0;
  
  for(unsigned int i = 0; i < this->NumberOfComponentsPerPixel; ++i)
    {
    difference += fabs(a[i] - b[i]);
    }
  return difference;
}


float SelfPatchCompare::SlowTotalAbsoluteDifference(const itk::ImageRegion<2>& sourceRegion)
{
  // This function assumes that all pixels in the source region are unmasked.
  
  // This method uses 3 iterators - one for the mask, and one for each image patch.
  // The entire mask is traversed looking for valid pixels, and then comparing the image pixels.
  // This is very inefficient because, since the target region stays constant for many thousands of patch
  // comparisons, the mask need only be traversed once. This method is performed by ComputeOffsets()
  // and PatchDifference*(). This function is only here for comparison purposes (to ensure the result of the other functions
  // is correct).

  itk::ImageRegionConstIterator<FloatVectorImageType> sourcePatchIterator(this->Image, sourceRegion);
  itk::ImageRegionConstIterator<FloatVectorImageType> targetPatchIterator(this->Image, this->TargetRegion);
  itk::ImageRegionConstIterator<Mask> maskIterator(this->MaskImage, this->TargetRegion);

  float sumDifferences = 0;
  this->NumberOfPixelsCompared = 0;
  
  float difference = 0;
  
  while(!sourcePatchIterator.IsAtEnd())
    {
    itk::Index<2> currentPixel = maskIterator.GetIndex();
    if(this->MaskImage->IsValid(currentPixel))
      {
      //std::cout << "Offset from iterator: " << this->Image->ComputeOffset(maskIterator.GetIndex()) * componentsPerPixel;
      FloatVectorImageType::PixelType sourcePixel = sourcePatchIterator.Get();
      FloatVectorImageType::PixelType targetPixel = targetPatchIterator.Get();
            
      difference = PixelDifference(sourcePixel, targetPixel);
      sumDifferences +=  difference;

      this->NumberOfPixelsCompared++;
      }

    ++sourcePatchIterator;
    ++targetPatchIterator;
    ++maskIterator;
    } // end while iterate over sourcePatch

  return sumDifferences;
}


float SelfPatchCompare::SlowTotalSquaredDifference(const itk::ImageRegion<2>& sourceRegion)
{
  // This function assumes that all pixels in the source region are unmasked.

  // This method uses 3 iterators - one for the mask, and one for each image patch.
  // The entire mask is traversed looking for valid pixels, and then comparing the image pixels.
  // This is very inefficient because, since the target region stays constant for many thousands of patch
  // comparisons, the mask need only be traversed once. This method is performed by ComputeOffsets()
  // and PatchDifference*(). This function is only here for comparison purposes (to ensure the result of the other functions
  // is correct).

  itk::ImageRegionConstIterator<FloatVectorImageType> sourcePatchIterator(this->Image, sourceRegion);
  itk::ImageRegionConstIterator<FloatVectorImageType> targetPatchIterator(this->Image, this->TargetRegion);
  itk::ImageRegionConstIterator<Mask> maskIterator(this->MaskImage, this->TargetRegion);

  float sumSquaredDifferences = 0;
  this->NumberOfPixelsCompared = 0;

  float squaredDifference = 0;

  while(!sourcePatchIterator.IsAtEnd())
    {
    itk::Index<2> currentPixel = maskIterator.GetIndex();
    if(this->MaskImage->IsValid(currentPixel))
      {
      //std::cout << "Offset from iterator: " << this->Image->ComputeOffset(maskIterator.GetIndex()) * componentsPerPixel;
      FloatVectorImageType::PixelType sourcePixel = sourcePatchIterator.Get();
      FloatVectorImageType::PixelType targetPixel = targetPatchIterator.Get();

      squaredDifference = PixelSquaredDifference(sourcePixel, targetPixel);

      //std::cout << "Source pixel: " << sourcePixel << " target pixel: " << targetPixel << "Difference: " << difference << " squaredDifference: " << squaredDifference << std::endl;

      sumSquaredDifferences +=  squaredDifference;

      this->NumberOfPixelsCompared++;
      }

    ++sourcePatchIterator;
    ++targetPatchIterator;
    ++maskIterator;
    } // end while iterate over sourcePatch

  return sumSquaredDifferences;
}


float SelfPatchCompare::SlowAverageAbsoluteDifference(const itk::ImageRegion<2>& sourceRegion)
{
  // This function assumes that all pixels in the source region are unmasked.

  // This method uses 3 iterators - one for the mask, and one for each image patch.
  // The entire mask is traversed looking for valid pixels, and then comparing the image pixels.
  // This is very inefficient because, since the target region stays constant for many thousands of patch
  // comparisons, the mask need only be traversed once. This method is performed by ComputeOffsets()
  // and PatchDifference*(). This function is only here for comparison purposes (to ensure the result of the other functions
  // is correct).

  itk::ImageRegionConstIterator<FloatVectorImageType> sourcePatchIterator(this->Image, sourceRegion);
  itk::ImageRegionConstIterator<FloatVectorImageType> targetPatchIterator(this->Image, this->TargetRegion);
  itk::ImageRegionConstIterator<Mask> maskIterator(this->MaskImage, this->TargetRegion);

  float sumDifferences = 0;
  this->NumberOfPixelsCompared = 0;

  float difference = 0;

  while(!sourcePatchIterator.IsAtEnd())
    {
    itk::Index<2> currentPixel = maskIterator.GetIndex();
    if(this->MaskImage->IsValid(currentPixel))
      {
      //std::cout << "Offset from iterator: " << this->Image->ComputeOffset(maskIterator.GetIndex()) * componentsPerPixel;
      FloatVectorImageType::PixelType sourcePixel = sourcePatchIterator.Get();
      FloatVectorImageType::PixelType targetPixel = targetPatchIterator.Get();

      difference = PixelDifference(sourcePixel, targetPixel);
      sumDifferences +=  difference;

      this->NumberOfPixelsCompared++;
      }

    ++sourcePatchIterator;
    ++targetPatchIterator;
    ++maskIterator;
    } // end while iterate over sourcePatch

  float averageDifferences = sumDifferences / static_cast<float>(this->NumberOfPixelsCompared);
  return averageDifferences;
}


float SelfPatchCompare::SlowAverageSquaredDifference(const itk::ImageRegion<2>& sourceRegion)
{
  // This function assumes that all pixels in the source region are unmasked.

  // This method uses 3 iterators - one for the mask, and one for each image patch.
  // The entire mask is traversed looking for valid pixels, and then comparing the image pixels.
  // This is very inefficient because, since the target region stays constant for many thousands of patch
  // comparisons, the mask need only be traversed once. This method is performed by ComputeOffsets()
  // and PatchDifference*(). This function is only here for comparison purposes (to ensure the result of the other functions
  // is correct).

  itk::ImageRegionConstIterator<FloatVectorImageType> sourcePatchIterator(this->Image, sourceRegion);
  itk::ImageRegionConstIterator<FloatVectorImageType> targetPatchIterator(this->Image, this->TargetRegion);
  itk::ImageRegionConstIterator<Mask> maskIterator(this->MaskImage, this->TargetRegion);

  float sumSquaredDifferences = 0;
  this->NumberOfPixelsCompared = 0;

  float squaredDifference = 0;

  while(!sourcePatchIterator.IsAtEnd())
    {
    itk::Index<2> currentPixel = maskIterator.GetIndex();
    if(this->MaskImage->IsValid(currentPixel))
      {
      //std::cout << "Offset from iterator: " << this->Image->ComputeOffset(maskIterator.GetIndex()) * componentsPerPixel;
      FloatVectorImageType::PixelType sourcePixel = sourcePatchIterator.Get();
      FloatVectorImageType::PixelType targetPixel = targetPatchIterator.Get();

      squaredDifference = PixelSquaredDifference(sourcePixel, targetPixel);

      //std::cout << "Source pixel: " << sourcePixel << " target pixel: " << targetPixel << "Difference: " << difference << " squaredDifference: " << squaredDifference << std::endl;

      sumSquaredDifferences +=  squaredDifference;

      this->NumberOfPixelsCompared++;
      }

    ++sourcePatchIterator;
    ++targetPatchIterator;
    ++maskIterator;
    } // end while iterate over sourcePatch

  float averageSquaredDifferences = sumSquaredDifferences / static_cast<float>(this->NumberOfPixelsCompared);
  return averageSquaredDifferences;
}

void SelfPatchCompare::ComputePatchScores()
{
  ComputeSourcePatches();
  for(unsigned int i = 0; i < this->SourcePatches.size(); ++i)
    {
    float totalAbsoluteScore = SlowTotalAbsoluteDifference(this->SourcePatches[i].Region);
    float averageAbsoluteScore = SlowAverageAbsoluteDifference(this->SourcePatches[i].Region);
    float totalSquaredScore = SlowTotalSquaredDifference(this->SourcePatches[i].Region);
    float averageSquaredScore = SlowAverageSquaredDifference(this->SourcePatches[i].Region);
  
    if(this->NumberOfPixelsCompared == 0)
      {
      std::cerr << "No pixels were compared!" << std::endl;
      return;
      }

    this->SourcePatches[i].TotalAbsoluteScore = totalAbsoluteScore;
    this->SourcePatches[i].AverageAbsoluteScore = averageAbsoluteScore;
    this->SourcePatches[i].TotalSquaredScore = totalSquaredScore;
    this->SourcePatches[i].AverageSquaredScore = averageSquaredScore;
    this->SourcePatches[i].Id = i;
    }
    
  std::sort(this->SourcePatches.begin(), this->SourcePatches.end(), SortByTotalAbsoluteScore);
}
