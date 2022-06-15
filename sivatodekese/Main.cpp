# include <Siv3D.hpp>
#include <fstream>

void ReadBinary(std::string& data, const std::string& path) {
	data = "";
	std::ifstream ifs(path, std::ios::binary);
	if (!ifs) {
		return;
	}
	data = std::string{ std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>() };
}

void CreateWav(const std::string& path, const std::string& data) {
	std::ofstream ofs(path, std::ios::binary);
	ofs << data;
}

void Main()
{
	//マイクをセットアップ 0.5秒間隔で録音する ループして録音する すぐに開始
	Microphone mic(0.5s, Loop::Yes, StartImmediately::Yes);
	
	if (System::EnumerateMicrophones().isEmpty())
	{
		throw Error{ U"No microphone is connected" };
	}

	int frame = 0;
	int index = 0;
	FFTResult fft;
	Audio audio(U"wavs\\hoge.wav");
	audio.play();
	while (System::Update())
	{
		frame++;
		// 結果を可視化 ここはみなくてよいk
		mic.fft(fft);
		for (auto i : step(800))
		{
			const double size = Pow(fft.buffer[i], 0.6f) * 1200;
			RectF(Arg::bottomLeft(i, 600), 1, size).draw(HSV(240 - i));
		}
		//50フレームごとに録音
		if (frame > 50) {
			//DEBUG用メッセージ
			Print << U"frame";
			//マイクで得た音声をファイルに書き出す
			mic.getBuffer().save(U"wavs\\dest.wav");
			mic.stop();
			audio.stop();
			//ここで書きだしたファイルを読みこんで再生
			//本番はサーバーから送られてきたファイルを読みこむ
			audio = Audio(U"wavs\\dest.wav");
			audio.play();
			mic.start();
			frame = 0;
		}
	}
}
