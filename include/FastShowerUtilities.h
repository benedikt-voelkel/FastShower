#include <vector>
#include <string>
#include <unordered_map>
#include <functional>


namespace utilities
{
  template <typename T>
  void insertIntoVector(std::vector<T>& v, int bin, T weight = T(1), T defaultValue = T(0))
  {
    if(bin < 0) {
      std::cerr << "Bin must be > -1" << std::endl;
      exit(1);
    }
    if(bin >= v.size()) {
      v.resize(bin + 1, defaultValue);
    }
    v[bin] += weight;
  }

  template <typename T, typename H>
  void vectorToHistogram(const std::vector<T> v, H& histo)
  {
    int sum = 0;
    for(int i = 0; i < v.size(); i++) {
      histo.SetBinContent(i+1, v[i]);
    }
  }

  template <typename T, typename H, typename F>
  void vectorToHistogram(const std::vector<T> v, H& histo, F sumEntries)
  {
    int sum = 0;
    for(int i = 0; i < v.size(); i++) {
      sum += sumEntries(v[i]);
      histo.SetBinContent(i+1, v[i]);
    }
    histo.SetEntries(sum);
  }

  template <typename K, typename V, typename H>
  void mapToHistogram(const std::unordered_map<K,V> m, H& histo)
  {
    histo.GetXaxis()->SetAlphanumeric();
    histo.GetXaxis()->SetCanExtend(true);

    for(auto& iter : m) {
      int bin = histo.GetXaxis()->FindBin(std::to_string(iter.first).c_str());
      histo.SetBinContent(bin, iter.second);
    }
    histo.LabelsOption(">", "X");
    histo.LabelsDeflate("X");
  }

  template <typename K, typename V, typename H, typename F>
  void mapToHistogram(const std::unordered_map<K,V> m, H& histo, F sumEntries)
  {
    histo.GetXaxis()->SetAlphanumeric();
    histo.GetXaxis()->SetCanExtend(true);

    int sum = 0;

    for(auto& iter : m) {
      int bin = histo.GetXaxis()->FindBin(std::to_string(iter.first).c_str());
      histo.SetBinContent(bin, iter.second);
      sum += sumEntries(iter.second);
    }
    histo.LabelsOption(">", "X");
    histo.LabelsDeflate("X");
    histo.SetEntries(sum);
  }

  template <typename K, typename V>
  void addToMap(std::unordered_map<K,V>& fillMap, K key, V value, V startValue = V(0))
  {
    if(fillMap.find(key) == fillMap.end()) {
      fillMap[key] = startValue;
    } else {
      fillMap[key] += value;
    }
  }
}
