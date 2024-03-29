#include "traincascade_features.h"
#include "cascadeclassifier.h"

using namespace std;

float calcNormFactor(const Mat& sum, const Mat& sqSum) {
    CV_DbgAssert(sum.cols > 3 && sqSum.rows > 3);
    Rect normrect(1, 1, sum.cols - 3, sum.rows - 3);
    size_t p0, p1, p2, p3;
    CV_SUM_OFFSETS(p0, p1, p2, p3, normrect, sum.step1())
    double area = normrect.width * normrect.height;
    const int* sp = (const int*)sum.data;
    int valSum = sp[p0] - sp[p1] - sp[p2] + sp[p3];
    const double* sqp = (const double*)sqSum.data;
    double valSqSum = sqp[p0] - sqp[p1] - sqp[p2] + sqp[p3];
    return (float) sqrt((double)(area * valSqSum - (double)valSum * valSum));
}

CvParams::CvParams() : name("params") {}
void CvParams::printDefaults() const
{ cout << "--" << name << "--" << endl; }
void CvParams::printAttrs() const {}
bool CvParams::scanAttr(const String prmName, const String val) { return false; }


//---------------------------- FeatureParams --------------------------------------

CvFeatureParams::CvFeatureParams() : maxCatCount(0) {
    name = CC_FEATURE_PARAMS;
}

void CvFeatureParams::init(const CvFeatureParams& fp) {
    maxCatCount = fp.maxCatCount;
}

void CvFeatureParams::write(FileStorage& fs) const {
    fs << CC_MAX_CAT_COUNT << maxCatCount;
}

bool CvFeatureParams::read(const FileNode& node) {
    if (node.empty()) {
        return false;
    }
    maxCatCount = node[CC_MAX_CAT_COUNT];
    return maxCatCount >= 0;
}

Ptr<CvFeatureParams> CvFeatureParams::create(int featureType) {
    return featureType == HAAR ? Ptr<CvFeatureParams>(new CvHaarFeatureParams) :
           featureType == LBP ? Ptr<CvFeatureParams>(new CvLBPFeatureParams) : Ptr<CvFeatureParams>();
}

//------------------------------------- FeatureEvaluator ---------------------------------------

void CvFeatureEvaluator::init(const CvFeatureParams* _featureParams,
                              int _maxSampleCount, Size _winSize) {
    CV_Assert(_maxSampleCount > 0);
    featureParams = (CvFeatureParams*)_featureParams;
    winSize = _winSize;
    numFeatures = 0;
    cls.create((int)_maxSampleCount, 1, CV_32FC1);
    generateFeatures();
}

void CvFeatureEvaluator::setImage(const Mat& img, uchar clsLabel, int idx) {
    CV_Assert(img.cols == winSize.width);
    CV_Assert(img.rows == winSize.height);
    CV_Assert(idx < cls.rows);
    cls.ptr<float>(idx)[0] = clsLabel;
}

Ptr<CvFeatureEvaluator> CvFeatureEvaluator::create(int type) {
    return type == CvFeatureParams::HAAR ? Ptr<CvFeatureEvaluator>(new CvHaarEvaluator) :
           type == CvFeatureParams::LBP ? Ptr<CvFeatureEvaluator>(new CvLBPEvaluator) : Ptr<CvFeatureEvaluator>();
}
