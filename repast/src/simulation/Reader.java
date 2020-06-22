package simulation;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import org.geotools.data.FileDataStore;
import org.geotools.data.FileDataStoreFinder;
import org.geotools.data.simple.SimpleFeatureCollection;
import org.geotools.data.simple.SimpleFeatureIterator;
import org.geotools.data.simple.SimpleFeatureSource;
import org.opengis.feature.simple.SimpleFeature;

public class Reader {

	public static List<SimpleFeature> loadGeometryFromShapefile(String filename) {
		File file = new File(filename);
		try {
			FileDataStore store = FileDataStoreFinder.getDataStore(file);
			SimpleFeatureSource featureSource = store.getFeatureSource();
			SimpleFeatureCollection featureCollection = featureSource.getFeatures();
			SimpleFeatureIterator featureIterator = featureCollection.features();
			ArrayList<SimpleFeature> simpleFeatures = new ArrayList<SimpleFeature>();
			while (featureIterator.hasNext())
				simpleFeatures.add(featureIterator.next());
			featureIterator.close();
			store.dispose();
			return simpleFeatures;
		} catch (IOException e) {
			e.printStackTrace();
		}
		return null;
	}

	public static HashMap<String, Object> loadEODMatrix(String filename) {
		HashMap<String, Object> out = new HashMap<String, Object>();
		HashMap<Integer, Integer> rows = new HashMap<Integer, Integer>();
		HashMap<Integer, Integer> columns = new HashMap<Integer, Integer>();
		ArrayList<ArrayList<Double>> eod = new ArrayList<ArrayList<Double>>();
		try {
			BufferedReader csvReader = new BufferedReader(new FileReader(filename));
			String line;
			String[] data;
			int i = 0;
			while ((line = csvReader.readLine()) != null) {
				data = line.split(";");
				if (i == 0) {
					for (int j = 1; j < data.length; j++) {
						columns.put(j - 1, Integer.parseInt(data[j]));
					}
				} else {
					rows.put(Integer.parseInt(data[0]), i - 1);
					ArrayList<Double> row = new ArrayList<Double>();
					for (int j = 1; j < data.length; j++) {
						row.add(Double.parseDouble(data[j]));
					}
					eod.add(row);
				}
				i++;
			}
			out.put("rows", rows);
			out.put("columns", columns);
			out.put("eod", eod);
			csvReader.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
		return out;
	}

	public static HashMap<String, Object> loadEODWalksMatrix(String filename) {
		HashMap<String, Object> out = new HashMap<String, Object>();
		HashMap<Integer, Integer> rows = new HashMap<Integer, Integer>();
		ArrayList<Double> walks = new ArrayList<Double>();
		try {
			BufferedReader csvReader = new BufferedReader(new FileReader(filename));
			String line;
			String[] data;
			boolean first = true;
			String sep = ";";
			int i = 0;
			while ((line = csvReader.readLine()) != null) {
				if (first) {
					if (line.charAt(0) == ',')
						sep = ",";
					first = false;
				}
				data = line.split(sep);
				if (i != 0) {
					rows.put(Integer.parseInt(data[0]), i - 1);
					walks.add(Double.parseDouble(data[i]));
				}
				i++;
			}
			out.put("rows", rows);
			out.put("walks", walks);
			csvReader.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
		return out;
	}

}