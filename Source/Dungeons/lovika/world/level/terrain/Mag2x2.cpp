#include "Dungeons.h"
#include "Mag2x2.h"

void BlockMag2x2::update(const TerrainGrid<terrain::Block>& data, int x, int y, TerrainGrid<terrain::Block>& mag) {
	if (x < 0 || y < 0) {
		return;
	}

	x &= 0xFFFFFFFE;
	y &= 0xFFFFFFFE;

	const auto& sample0 = data.get(x, y);
	//const auto& sample1 = data.get(x, y + 1);
	//const auto& sample2 = data.get(x + 1, y);
	//const auto& sample3 = data.get(x + 1, y + 1);

	const auto average = [&] {
		return sample0; // there's no way to average blocks
		// alternative is to choose the most popular one (if they happen to repeat)
	}();

	mag.set(x / 2, y / 2, average);
}

void HeightMag2x2::update(const TerrainGrid<terrain::Height>& data, int x, int y, TerrainGrid<terrain::Height>& mag)
{
	if (x < 0 || y < 0) {
		return;
	}

	x &= 0xFFFFFFFE;
	y &= 0xFFFFFFFE;

	const auto& sample0 = data.get(x, y);
	const auto& sample1 = data.get(x, y + 1);
	const auto& sample2 = data.get(x + 1, y);
	const auto& sample3 = data.get(x + 1, y + 1);

	const auto average = [&] {
		return (sample0 + sample1 + sample2 + sample3) / 4;
	}();

	mag.set(x / 2, y / 2, average);
}

void TerrainMag2x2::update(const TerrainGrid<terrain::Type>& data, int x, int y, TerrainGrid<terrain::Type>& mag) {
	if (x < 0 || y < 0) {
		return;
	}

	x &= 0xFFFFFFFE;
	y &= 0xFFFFFFFE;

	const auto& sample0 = data.get(x, y);
	const auto& sample1 = data.get(x, y + 1);
	const auto& sample2 = data.get(x + 1, y);
	const auto& sample3 = data.get(x + 1, y + 1);

	const auto average = [&] {
		if (sample0.isBoundary() || sample1.isBoundary() || sample2.isBoundary() || sample3.isBoundary()) {
			return TerrainCell::boundary;
		}

		const auto killzoneCount =
			static_cast<int>(sample0.isKillzone()) +
			static_cast<int>(sample1.isKillzone()) +
			static_cast<int>(sample2.isKillzone()) +
			static_cast<int>(sample3.isKillzone());

		if (killzoneCount > 2) {
			return TerrainCell::boundary;
			// stop displaying killzones alltogether in the map; mag2x2 is only used for the map; good enough hack
			/*
			if (sample0.isBelow() || sample1.isBelow() || sample2.isBelow() || sample3.isBelow()) {
				return TerrainCell::killzoneBelow;
			} else {
				return TerrainCell::killzone;
			}
			*/
		} else {
			return TerrainCell::unset;
		}
	}();

	mag.set(x / 2, y / 2, average);
}

void RevealMag2x2::update(const TerrainGrid<terrain::Reveal>& data, int x, int y, TerrainGrid<terrain::Reveal>& mag) {
	if (x < 0 || y < 0) {
		return;
	}

	x &= 0xFFFFFFFE;
	y &= 0xFFFFFFFE;

	const auto& sample0 = data.get(x, y);
	const auto& sample1 = data.get(x, y + 1);
	const auto& sample2 = data.get(x + 1, y);
	const auto& sample3 = data.get(x + 1, y + 1);

	const auto average = [&] {
		const auto fogCount = (sample0 == RevealCell::Fog ? 1 : 0) +
			(sample1 == RevealCell::Fog ? 1 : 0) +
			(sample2 == RevealCell::Fog ? 1 : 0) +
			(sample3 == RevealCell::Fog ? 1 : 0);

		const auto partialCount = (sample0 == RevealCell::PartialReveal ? 1 : 0) +
			(sample1 == RevealCell::PartialReveal ? 1 : 0) +
			(sample2 == RevealCell::PartialReveal ? 1 : 0) +
			(sample3 == RevealCell::PartialReveal ? 1 : 0);

		const auto fullCount = (sample0 == RevealCell::FullReveal || sample0 == RevealCell::Covered ? 1 : 0) +
			(sample1 == RevealCell::FullReveal || sample1 == RevealCell::Covered ? 1 : 0) +
			(sample2 == RevealCell::FullReveal || sample2 == RevealCell::Covered ? 1 : 0) +
			(sample3 == RevealCell::FullReveal || sample3 == RevealCell::Covered ? 1 : 0);

		return fogCount > partialCount || fogCount > fullCount ? RevealCell::Fog
			: partialCount > fullCount ? RevealCell::PartialReveal
			: RevealCell::FullReveal;
	}();

	mag.set(x / 2, y / 2, average);
}
