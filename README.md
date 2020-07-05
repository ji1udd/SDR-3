# SDR-3 transceiver improvement

## CW���[�h�AS���[�^�[�A�E�H�[�^�[�t�H�[���\���Ȃǂ̋@�\�ǉ�
- �n�[�h�E�F�A����ɂ��ẮA[wiki](https://github.com/ji1udd/SDR-3/wiki) ���Q�Ƃ��ĉ������B
- �ԊO: SDR-3A��� �I���W�i�������DDC/DUC���(��H��Herms-Lite2���Q�l)�Œu��������HF�}���`�o���_(160M-10M) �ɂȂ�܂����B���M�o��5W(2020�N6��)�B
<img src="hardware/Doc/Replace_SDR3A_with_DDC-DUC_board.jpg" width="320px">
https://github.com/ji1udd/SDR-3/wiki/7_SDR-3-%E3%82%92DDC-DUC%E5%8C%96/

### �͂��߂�
- �\�t�g�E�F�A�J�����̍\�z���@�͂�������H�[��HP�Ɍf�ڂ���Ă��܂��B
- �g�p���Ă��� SDR-3 �\�[�X�R�[�h�̓g�����W�X�^�Z�p 2018�N 9���� �t�^DVD-ROM�ɔ[�߂��Ă��܂��B
- �ǉ���Ƃ� �eStep���Ɂu�ύX������m�F�v���s�Ȃ��Ă��������B1�O��Step�̃t�@�C����ۑ����Ă����Ə�肭���삵�Ȃ����ɕύX�O��̃t�@�C���̔�r���ł�����A��߂肪�e�ՂɂȂ�܂��B
- �eStep�̃t�H���_�� jpeg �� txt ���g�Ŋi�[����Ă��܂��Bjpeg �͕ύX�O��̃t�@�C�����e�̔�r���ʂŕύX�ӏ��������Ă��܂��Btxt�t�@�C�������p���ăt�@�C�����C�����܂��B
- �g���u������̂��߁Amake clean ���s���Ă��� make ���Ă��������Biambic.o �͌ʂɍ폜���邩�Amakefile���C�����đΏ����Ă��������B
- Flash Loader Demonstrator ���g�p���� STM32�ɏ������݂��s���ۂɂ́uGlobal Erase�v�̃��W�I�{�^����I�����ĉ������B

### Step1:
- ��������H�[��HP�Ɍf�ڂ���Ă���p�b�`�̓K�p

### Step2:
- CW���[�h(CW Lower �� CW Upper)��ǉ�
- CW��M����600Hz�ɂȂ�悤�Ƀ`���[�j���O����ƁALCD��ɕ\������Ă�����g�������M���g���ƂȂ�B
- PTT�������Ƒ��M�ƂȂ�B
- CW�g��ON/OFF���ɑ��֐��͎g�p���Ă��Ȃ�(CW�g�͋}�s�ɗ����オ��/�����肷��)�B

### Step3:
- CW�L�[�C���O�@�\��ǉ�
- CW key �� PA5 �� �g�p�BR2 �� 3.3k������������BPA5��10pin�̃s���w�b�_�̘e�ɒ[�q���z�u����Ă���B
- PTT �ő��M��Ԃɂ��āAPA5��GND�ɗ��Ƃ���CW�g�̑��M�ƂȂ�B
- CW�g��ON/OFF�͑��֐�(raised cosine)���g�p(CW�g�͊ɂ₩�ɗ����オ��/�����肷��)�B

### Step4:
- iambic�L�[���[�@�\��ǉ�
- Dot key �� PA5 ���A Dash key �� PA6 ���g�p�B R3 �� 3.3k������������BPA6��10pin�̃s���w�b�_�̘e�ɒ[�q���z�u����Ă���B
- PA5/PA6��GND�ɗ��Ƃ���Dot/Dash��CW�g�����M����� (PTT�̑���͕s�v)�B
- USB�[�q(PA11)��Sidetone�M�����o�͂����(500Hz ��`�g)�̂ň��d�X�s�[�J�[���Ȃ��Ń��j�^�ł���BPA11�ƈ��d�X�s�[�J�Ƃ̊Ԃɂ͒���ɒ�R(1k���Ȃ�)������B
- Keyer�̃X�s�[�h�ƃu���[�N�C�����Ԃɂ��ă��j���[�͖������B�ύX�������ꍇ�́A �\�[�X�R�[�h�� define �̒l���C������K�v������B
- keyer�t�H���_��SSB_trx�t�H���_�ɓ����

### Step5:
- iambic�L�[���[��Sidetone���g����600Hz�ɕύX(��M���̃g�[�����g���ƈ�v������)
- Sidetone���I�[�f�B�I�R�[�f�b�NIC�o�R�̃w�b�h�t�H���o�͂ɕύX
- PA11�̏o�͂�RC��LPF��ʂ��ăI�[�f�B�I�R�[�f�b�N��IN1L/R�ɓ��͂���B
[_RC LPF Schematic_](hardware/SDR-3_SideTone.pdf)
[_Photo_](hardware/SDR-3_sidetone.JPG)
- Sidetone�̉��ʒ����ɂ��ă��j���[�͖������B�ύX�������ꍇ�́A tlv320aic3204.c �̏C�����e��ύX����K�v������B
- Step5�t�H���_���ɂ���keyer�t�H���_���g�� (keyer�t�H���_�������ւ���K�v����B���e�X�V����)

### Step6:
- setting���j���[�ɁA���L��iambic�L�[���[�̐ݒ荀�ڂ�ǉ�
- Speed�F10�`40 WPM
- Break-in ���ԁF 50�`500 ms (���ۂ́A���\ms�̌덷����)
- Sidetone ���ʁF 0 �` 117
- Step6�t�H���_���ɂ���keyer�t�H���_���g�� (keyer�t�H���_�������ւ���K�v����B���e�X�V����)
- radio.c �ɂ����� CW���[�h�ǉ����̃o�O�C��

### Step7:
- �L�[���[�Ƀo�O�L�[/�X�g���[�g�L�[�̋@�\��ǉ�
- setting���j���[�� iambic �Ƃ��Ďg�p���邩 �o�O�L�[/�X�g���[�g�L�Ƃ��Ďg�p���邩��I���ł���
- �X�g���[�g�L�[���g�p����ꍇ�́A�X�g���[�g�L�[�� Dash key (PA6) ���ɐڑ�����
- Step7�t�H���_���ɂ���keyer�t�H���_���g�� (keyer�t�H���_�������ւ���K�v����B���e�X�V����)

### Step8:
- Transceiver ��ʂŃL�[���[�̃X�s�[�h��ݒ�ł���悤�ɕύX
- CW���M���́A���[�^���[�G���R�[�_ #2 �̉�]�ɂ��X�s�[�h���ύX�ł���
- ��M���́A���[�^���[�G���R�[�_ #2�̃v�b�V���X�C�b�`��(2��)������ "Keyer SPD" ��I�񂾌�A���[�^���[�G���R�[�_ #2�̉�]�ɂ��X�s�[�h���ύX�ł���
- Step8�ł́Akeyer�t�H���_���e�̕ύX�͂Ȃ�

### Step9:
- �I�t�o���h����setting���j���[�\�����ɃL�[���[�𑀍삵������Sidetone���R�ꕷ������Ǐ�̑΍�
- Step9�t�H���_���ɂ���keyer�t�H���_���g�� (keyer�t�H���_�������ւ���K�v����B���e�X�V����)

### Step10:
- setting���j���[�� CW�s�b�`���g���̐ݒ荀�ڂ�ǉ�
- �X�y�N�g�����\������ CW���[�h���̃`���[�j���O�|�C���g�������c�̐ԃ��C����ǉ�
- CW�s�b�`���g���̐ݒ�͈͂� 500Hz �` 800Hz�B 10Hz�P�ʂŕύX�\
- CW��M�����D�݂̃g�[���ƂȂ�悤�ɐݒ肷���Sidetone�̃g�[�����g�����A�����ĕύX�����B���l�ɃX�y�N�g�����\�����̃`���[�j���O�|�C���g�������ԃ��C���̈ʒu���A�����ĕύX�����B
- Step10�ł́Akeyer�t�H���_���e�̕ύX�͂Ȃ�

### Step11:
- CW�p��M�t�B���^�̒ǉ�
- LPF: CW���[�h���A600Hz, 700Hz, 800Hz, 900Hz, 1kHz �� 5��ނ�I���������� 8��IIR�t�B���^���g�p����BCW���[�h�ȊO�̎��܂���CW���[�h���ł��J�b�g�I�t���g����2.2kHz�����LPF��I�������ꍇ�� �f�t�H���g��6��IIR�t�B���^���g�p����B
- HPF: CW���[�h���A8��IIR�t�B���^���g�p�BCW���[�h�ȊO�̎��� �f�t�H���g��2��IIR�t�B���^���g�p����B
- CW���[�h���A�t�B���^�ʉߑш悪�킩��悤�ɃX�y�N�g�����\���̃n�C���C�g�̈悪 HPF�̑I���ŕω�����悤�ɂ����B
- �e�t�B���^�W���l�͂��������x���B
- Step11�ł́Akeyer�t�H���_���e�̕ύX�͂Ȃ�
- �o�O�C����Step26��K�p���Ă��������B

### Step12:
- CW���M�g�̑ш敝�̍L��������P
- CW���M�g�̗����オ��/�������莞�Ԃ��]���� 5.3ms(fs=12kHz 64�T���v����)����A8ms (96�T���v����)�ɕύX�B
- Step12�ł́Akeyer�t�H���_���e�̕ύX�͂Ȃ�

### Step13:
- �g�����V�[�o�[��ʂ̕ύX�B���e�͉��L�B
- �A�i���O�^�C�v��S���[�^�[�\����ǉ�
- F.Step�\���̒ǉ�
- ��L�ɔ����A���̃��j���[���ڂ̕\���̏��T�C�Y��
- Step13�ł́Akeyer�t�H���_���e�̕ύX�͂Ȃ�

### Step14:
- S���[�^�[�̕����Ղ̕`����@��ύX
- ��96�h�b�g�~�c47�h�b�g�̃r�b�g�}�b�v�f�[�^���g�p����S���[�^�[�̕����Ղ�`��
- �����Ղ̃r�b�g�}�b�v�f�[�^�쐬�c�[���Ƃ���Excel�V�[�g���g�p (firmware/S-meter Panel Data Generator/s-meter-panel_data_generator.xls)
- Step14�ł́Akeyer�t�H���_���e�̕ύX�͂Ȃ�

### Step15:
- S���[�^�[�̎�ނƂ��� ��ǂ��񂪃f�U�C�����ꂽS���[�^�[���^�C�v1�Ƃ��Ēǉ��B�I���W�i���̓^�C�v0�B
- S���[�^�[�̎w�j�̐F��8�F(���A�A�ԁA�΁A�V�A���A�}�[���^�A���A��)�̒�����I���ł���B
- �^�C�v1�̏ꍇ�́AS���[�^�[�̎w�j�̑�����I���ł���(��������͓̂J������������̃A�C�f�A)
- setting ���j���[�ł�����S���[�^�[�^�C�v�A�w�j�̐F�A������ݒ�ł���B
- �^�C�v1�ɃR�[���T�C���∤�̂Ȃǂ�\���������ꍇ�́AExcel�V�[�g(s-meter-panel_data_generator_r1(��ǂ����).xls)���g�p����B�t���[�n���h�ŕ�����`���Ă��ǂ����Afont8.c���Q�l�ɂ���Ƒ������ƕ����f�U�C��������ł���B
- Step15�ł́Akeyer�t�H���_���e�̕ύX�͂Ȃ�

### Step16:
- S���[�^�[�̎�ނƂ��� �X��2�^�C�v���ǉ��B���v4�^�C�v�B
- �^�C�v2�́A�܂��̂���f�U�C���BExcel�V�[�g�́As-meter-panel_data_generator_r1(�܂��̂����).xls (BBS�̏������ݏ������ɂ�ǂ�����Excel�t�@�C�������ύX���č쐬)
- �^�C�v3�́A����ӂ񂳂�f�U�C���BExcel�V�[�g�́As-meter-panel_data_generator_r2(����ӂ񂳂��).xls
- �^�C�v3�̏ꍇ�́Asetting���j���[�� Needle bold ���w�肷��ƒʏ�̃o�[�\���ɂł���B
- Excel�V�[�g��C����p�z��f�[�^�̐������Ɍ�肪�������Bgit��upload���Ă���S�Ă�Excel�V�[�g���C��
- Step16�ł́Akeyer�t�H���_���e�̕ύX�͂Ȃ�

### Step17:
- Step12�̏C���ō������Ă������L2�̃o�O�̏C��
- �������[�ǂݏo����Ch�ύX�̍ۂ�LPF��HPF���������\������Ȃ��B
- �V���A���[�q�ɏo�͂���郍�O�ɁuERR:lcd_draw_ch�v���\�������B

### Step18:
- ���M���Ƀ��[�^�[�����삷�郂�[�h��ǉ��B���[�v�o�b�N��M��Tx Monitor�@�\���g�p���Ď������Ă���B
- setting���j���[�� TxM mode (Tx Meter Mode) �� 3���[�h��I���ł���B
- ���[�h0 �́A���M���Ƀ��[�^�[�𓮍삳���Ȃ�(OFF)���[�h
- ���[�h1 �́A���M������M���Ɠ����p�l�����g���ĕ\�����s�����[�h�B���[�^�[�X�P�[���́A��M���Ɠ��� -14dBu�`+100dBu�B
- ���[�h2 �́A���M�p�̃p�l�����g���ĕ\�����s�����[�h�B�p�l���^�C�v0�̗�ł́A���[�^�[�X�P�[����+65dBu�`+85dBu�Ƃ��Ă���A�O�t�����j�A�A���v�̃Q�C����25�`26dB��z�肵���p���[�̖ڐ���t���ƂȂ��Ă���B
- �p�l���^�C�v0�̑��M�p�p�l����� Excel�V�[�g�́As-meter-panel_data_generator_tx.xls
- ���ӁF���[�^�[�̃\�[�X�R�[�h�� radio.c ���番������ meter.c �Ƃ����Bmeter.c �� radio�t�H���_�ɓ���邱�ƁB

### Step19:
- ��ǂ���̃A�C�f�A�̓d���d�����j�^�\���@�\��ǉ��B
- �d���d�����ቺ��������Warning�@�\��ǉ� (�}�[���_�F�œd���d���l��\��)�B
- �d�����j�^�p��ADC���͂Ƃ��� PA7 ���g�p�B PA7��10pin�t���b�g�P�[�u���R�l�N�^��10�ԃs���ɃA�T�C������Ă���B
- PA7(ADC����)��Max. 3.3V�܂łȂ̂ŁA�d���d���ɍ�����R�������ǉ�����K�v������B
[_VBatt Schematic_](hardware/SDR-3_VBatt.pdf)
- �d���d���\���l�̈��艻�̂��� �A��10���ADC�ϊ����ʂ̕��ϒl��\���B
- setting���j���[�� "BattM mode" �� 3���[�h��I���ł���B
- ���[�h0 �́A�d���d���\�������Ȃ����[�h(�]���ʂ�A��M���x���݂̂�\�����郂�[�h)
- ���[�h1 �́A�d���d���݂̂�\�����郂�[�h�B�d���\��3��
- ���[�h2 �́A��M���x���Ɠd���d���̗�����\�����郂�[�h�B�d���\��2��
- radio.c �� #define �� BATT_FS �� BATT_LOW �̒l���e�l�̎g�p�����ɍ��킹�ďC�����邱�ƁB
- ���������F���[�^�[�p�l���^�C�v0 ��I�������ꍇ�A��M���x���\����d���d���\����������B
- ����: PA7�ɂ� 3.3V�𒴂���d�����|����Ȃ��悤�ɂ��邱�ƁB3.3V�𒴂���d�����|�����SDR-3������\�����ɂ߂č����B�t�F�C���Z�[�t�̂��߂Ƀc�F�i�[Di��PA7���N�����v����̂��ǂ����@�Ǝv����B

### Step20:
- SDR-3��PC�\�t�g�E�F�A�Ƃ̘A�g�@�\��ǉ��BCN3(UART1)�[�q��CI-V�v���g�R�����g�p�B
- setting���j���[�� CI-V�@�\�̗L��/������ݒ�B�L���ɂ����UART1��CV-I�ʐM�p�ƂȂ�A�����ɂ���Ə]���ʂ�f�o�b�O�p�V���A���ʐM(�^�[�~�i���ڑ�)�ƂȂ�B
- Turbo HAMLOG �� Ctestwin �� SDR-3�̎��g���ƃ��[�h(SSB,CW,AM) ����荞�߂�BPC�\�t�g�E�F�A����SDR-3�̐���͖��Ή��B
- HAMLOG�Ή��̂��� UART1�̃{�[���C�g�� 115200bps ���� 38400bps �ɕύX(�⑫�FSDR-3�t�@�[���E�F�A�̏������ݎ��͏]���ʂ�{�[���C�g�͎����ݒ肳���)
- CI-V�v���g�R���ɂ�����SDR-3�̃A�h���X�� 0x10 (���X��IC-275�p�̊��蓖��)�BSDR-3�̃A�h���X�́Arigctrl.c �� #define�ŕύX�\�B
- S���[�^�[�p�l���^�C�v3 ��Bold���[�h�Ƀs�[�N�z�[���h�@�\��ǉ�(����ӂ񂳂��)
- S���[�^�[�p�l���^�C�v0 �̎�M���x���\����d���d���\���̕����̂����(Step19�̐�������)�����P
- Step20�t�H���_���ɂ��� rigctrl.c �� radio�t�H���_�ɓ����B
- Step20�t�H���_���ɂ��� meter.c ���g�� ( meter.c �������ւ���K�v����B���e�X�V����) 

### Step21:
- Waterfall�\���̒ǉ�
- setting���j���[�� Waterfall�\����ON/OFF��ݒ�ł���BON���̓X�y�N�g�����\���� 7div�\���ƂȂ�X�y�N�g�����\��������Waterfall���\�������BOFF���͏]���ʂ�X�y�N�g������ 10div�\���ƂȂ�B
- Waterfall�̕\���F��112�K���ŁA�X�y�N�g�����̍ŏ��l(�\��0dot)�����A�X�y�N�g���l(�\��dot��;Max 83)���傫���Ȃ�ɏ]���� ��>��>�V�A��>��>��>��>�}�[���^>���ɋ߂��F �ƂȂ�B
- ���[�^���[�G���R�[�_#1�̃v�b�V���X�C�b�`��(2��)������ "WF OFS" ��I�񂾌�A���[�^���[�G���R�[�_ #1�̉�]�ɂ��A�X�y�N�g�����l(�\��dot��)�ɑ΂���\���F�̃I�t�Z�b�g��ݒ�ł���B�ݒ�\�͈͂� -20 ���� 100�܂ŁB
- ���[�^���[�G���R�[�_#1�̃v�b�V���X�C�b�`��(3��)������ "WF Step" ��I�񂾌�A���[�^���[�G���R�[�_ #1�̉�]�ɂ��A�X�y�N�g�����l(�\��1dot)�ɑ΂���F�~���̃X�e�b�v��ݒ�ł���B�ݒ�\�͈͂� 1 ���� 5�܂ŁB
- "WF OFS"��"WF Step"�ASW1���j���[��"11:SPEANA REF"�����p���čD�݂̃X�y�N�g�����\��/Waterfall�\���ɐݒ肷��B
- "WF OFS"��"WF Step"�Őݒ肵���l�̓`�����l���������ɕۑ��ł���B
- Step21�t�H���_���ɂ��� waterfall.c �� SSB_trx\board\lcd\controller�t�H���_�ɓ����B

### Step22:
- ����ӂ񂳂�l�ẴX�P���`�@�\�̒ǉ��B�X�P���`�@�\�̎d�l�͂�������H�[BBS�̂���ӂ񂳂񓊍e�L���u�ȈՃX�P���`�@�\�v���Q�Ƃ̂��ƁB
- ����ӂ񂳂�\�[�X�R�[�h���x�[�X�ɉ��L�̕ύX���������B
- GUI�̃{�^���@�\���g�p���� SQL���x����Busy/SQL��\���B
- �~���[�g�I���p�^�C�}�̒ǉ��B�~���[�g�I��/�I�t�Ԃ̑J�ڏ����͈ȉ��̒ʂ�B
- �~���[�g�I�t: �M�����x����SQL���x���ȏ�ɂȂ�����~���[�g�I�t����B
- �~���[�g�I��: �M�����x����SQL���x�������ƂȂ�K�莞�Ԃ��o�߂�����~���[�g�I������B�K�莞�Ԍo�ߑO�ɐM�����x����SQL���x���ȏ�ƂȂ����ꍇ�͌o�ߎ��Ԃ̓N���A�����B
- ���莞�Ԃ�ύX�������ꍇ�́Aradio.c ���� #define SQL_OFF_CNT �@�̒l ��ύX����B

### Step23:
- 10ms���̏����ɂ� time=4 �� �������W�����Ă��� bug �� time 2�`4�ɏ����𕪎U������10ms���������j�]���Ȃ��悤�ɏC��

### Step24:
- Step22�Ŏ��������X�P���`�@�\�̃o�O�C��
- �o�O�Ǐ�́A�u�X�P���`���|�����ăI�[�f�B�I�o�͂��~���[�g����Ă��鎞�Ƀ��[�^���[�G���R�[�_#2�ŉ��ʒ�������ƃX�P���`���|�����Ă�����(SQL�\��)�Ȃ̂ɃI�[�f�B�I���o�͂����v�B
- ���[�^���[�G���R�[�_#2�ŉ��ʒ����������͉��ʕω����킩��悤�Ƀ~���[�g����������Ƌ��ɓ�����������X�P���`���|�����Ă��Ȃ���ԂƂȂ�悤�ɏC���B
- �|�b�v�A�b�v�\������SQL/Busy�̕\�����X�V���Ȃ��悤�ɏC��(SQL Level�������̕\���̃`�����΍�)�B�|�b�v�A�b�v�\�����I��������SQL/Busy�\�����X�V����B

### Step25:
- 4ch �������[�L�[���[�@�\�̒ǉ�
- 1���b�Z�[�W�̒����͍ő�47����
- CW���[�h(CWL or CWU)���ɂ̂ݗL���ƂȂ�BSW5�`8���g�p����B
- SW�ƃ��b�Z�[�W�̑Ή�: SW5=���b�Z�[�W0, SW6=���b�Z�[�W1, SW7=���b�Z�[�W2, SW8=���b�Z�[�W3
- CW���[�h����SW5�`8 ��Z��������� ���b�Z�[�W�𑗐M����
- CW���[�h����SW5�`8 �𒷉�������� ���ꂼ��̃��b�Z�[�W�̃G�f�B�b�g���[�h�ƂȂ�
- �G�f�B�b�g���[�h���A���[�^���[�G���R�[�_#1����]������ƕ������I���ł���B���[�^���[�G���R�[�_#2����]������ƕҏW�ʒu�������J�[�\�����ړ�����B���[�^���[�G���R�[�_#1�̃v�b�V���X�C�b�`�������ƃJ�[�\���ʒu�ɕ���(�X�y�[�X)���}�������B���[�^���[�G���R�[�_#2�̃v�b�V���X�C�b�`�������ƃJ�[�\���ʒu�̕������폜�����B
- �G�f�B�b�g���[�h���� SW5�`8��Z��������� �G�f�B�b�g���[�h���甲���Ēʏ퓮��ɖ߂�B
- �ҏW�������b�Z�[�W�̓`�����l���������[�ɕۑ��ł���B�`�����l���������ɕۑ����Ȃ��Ɠd���I�t�ɂ��ҏW�������b�Z�[�W�͎�����B
- �`�����l���������[�̓��b�Z�[�W�ۑ��̂��߃I���W�i����100�Ǖ�����25�Ǖ��ɕύX�B
- 1�`�����l�����̃`�����l���������[�Ƀ��b�Z�[�W0�`3��4���b�Z�[�W���ۑ��ł���̂�25�ǂ̃������[���t���Ɏg�p�����100���b�Z�[�W���ۑ��ł���B
- Step25�t�H���_���ɂ��� msgedit.c �� radio�t�H���_�ɓ����B
- Step25�t�H���_���ɂ��� keyer�t�H���_���g�� (keyer�t�H���_�������ւ���K�v����B���e�X�V����)

### Step26:
- Step11�̃o�O�C��

### Step27:
- �`�F���l���������[��ǂݏo���ă������[�L�[���[���b�Z�[�W�ɑ΂��Ă̂ݐݒ肷��@�\�̒ǉ��B
- �ʏ�̃������[�ǂݏo�����[�h(SW2�Z����)�ƈقȂ�A���g���⃂�[�h�Ȃǂ͕ύX����Ȃ��B
- ���b�Z�[�W�G�f�B�b�g���[�h����SW2��Z�������邱�ƂŃ`�F���l���������[�ǂݏo�������b�Z�[�W�G�f�B�b�g���[�h�J�ڂ���B
- �eSW�ɂ�郂�[�h�J�ڂ���у��[�^���[�G���R�[�_#1�̋@�\�ALCD�\�����ڂɂ��ẮA StateDiagram_MessageEdit_ch-Memory.pdf ���Q�ƁB
- (2019.3.2) Step27�̃o�O�̑΍�p�b�`��ǉ��BStep27_additional_patch�t�H���_���̒ǉ��p�b�`�̓K�p�� Step27�t�H���_����keyer�t�H���_���g�p����BStateDiagram_MessageEdit_ch-Memory.pdf�������B
- ���b�Z�[�W���M����SW1/SW2��Z��������ƃ��b�Z�[�W���M���I������B
- ���b�Z�[�W�G�f�B�b�g���[�h�i��ch�������[�ǂݏo�����[�h)����SW5-SW8�̒Z���� �܂��� SW1�Z�����Œʏ탂�[�h�ɖ߂�B
- (2019.3.4) PTT���[�h�����[�����^���ɐݒ肵�Ă���ꍇ�� CW���[�h�ł� SW8�������̍� ����M�؂�ւ����듮�삷��o�O���C���BStep27_additional_patch_part2 �t�H���_���̒ǉ��p�b�`�̓K�p���邱�ƁB�⑫�����FCW���[�h���ASW8�̓��b�Z�[�W�������Ɋւ��鑀��ƂȂ�APTT�Ƃ��Ă͋@�\���܂���B

### Step28:
- ����M�؂�ւ����̉��L�o�O�̏C���B
- CW���M����CW��M�ɖ߂����^�C�~���O��CW���M�J�n����Ƒ��M�o�͂��~���[�g����Ă��܂����Ƃ�����B

### Step29:
- CI-V�@�\�̊g�����s�� PC ���� SDR-3 �� ���g��/���[�h(SSB,CW,AM)/����M�ؑ� �𐧌�ł���悤�ɂ����B
- setting���j���[�� CI-V�v���g�R���ɂ�����SDR-3�̃A�h���X(0�`255) �� PC�����CI-V�R�}���h�̃G�R�[�o�b�N���s�����ۂ� ��I���ł���B
- �f�t�H���g��SDR-3�A�h���X�� 0x94 (���X��IC-7300�p�̊��蓖��)�ɕύX�BWSJT-X�� PTT Method �Ƃ���CAT��I���ł���@��̃A�h���X�Ƃ������߁B
- WSJT-X : File��settings��Radio�^�u�́uTest CAT�v�ƁuTest PTT�v�� Pass���邱�Ƃ��m�F�ρB�A�� SDR-3�� VFO B ������Ă��Ȃ��Ȃǐ��񂪂��邽�� WSJT-X�̋@�\�ɐ��񂪂���Ǝv����B
- Hamlog/Ctestwin�ł̓G�R�[�o�b�N�Ȃ��ł����삷�邪�AWSJT-X�ł̓G�R�[�o�b�N����ɂ��Ȃ��ƁuTest CAT�v�ŃG���[�ƂȂ�B
- Step29�t�H���_���ɂ��� rigctrl.c �� radio�t�H���_�ɓ����B ( rigctrl.c �������ւ���K�v����B���e�X�V����)

### Step30:
- ExtPTT�o�͂̒ǉ��BExtPTT���g�p���ĊO���@���SDR-3�̑���M�ƘA�������邱�Ƃ��ł���BExtPTT�o�͂́A��M��Hi-Z�A���M��"L"���x���ƂȂ�B
- PA12���f�W�g��(DTC144)�̓��͂ɐڑ����A�f�W�g���̏o�͂��O���o�͂���B���@�̏ꍇ�́A�I�[�f�B�I�C���^�[�t�F�C�X����J2-4pin�ɐڑ��B
[_External PTT Schematic_](hardware/SDR-3_ExtPTT.pdf)
- Setting���j���[�� ExtPTT�o�͂�L���ɂ��邩�A����(=�O���@��A���I�t)�ɂ��邩��I���ł���B
- �����ɂ��邱�ƂŁAExtPTT�͏�� Hi-Z �ƂȂ�̂� J2�ɃX�e���I�C���z��(3�Ƀv���O)��}���Ē������Ƃ��ł���(PHONE-R��ExtPTT�������Ȃ��̂�)�B
- �L���ɂ���ꍇ�́A4�Ƀv���O���g�p����B


## ���̑�
### Option1:
- PHAS���[�h��SSB���M�������̕ϒ������̉��P��}���Ă݂��B�����̎����l�ɂ��Ă� [JA3GSE����̃z�[���y�[�W](http://www.ja3gse.sakura.ne.jp/SDR-3.html) �������������B
- Hilbert�ϊ��t�B���^�̃^�b�v���� 127 ���� 255 �ɕύX
- Hilbert�ϊ��t�B���^�̌W���l�Ƃ��� �p�X�o���h 96Hz�`5904Hz (���K�����g�� 0.008�`0.492) �Ő݌v�����l���g�p

### Option2:
- JA3GSE �҂��񂩂� �X�Ȃ�������P�ł𒸂����̂ł����Ɍf�ڂ��܂��B�����̎����l�ɂ��Ă� [JA3GSE����̃z�[���y�[�W](http://www.ja3gse.sakura.ne.jp/SDR-3.html) �������������B
- �����t�@�C���� Option1 ����̍���
- Hilbert�ϊ��t�B���^�̃^�b�v���� 511 �ɕύX
- Hilbert�ϊ��t�B���^�̌W���l�Ƃ��� �p�X�o���h 48Hz�`5952Hz (���K�����g�� 0.004�`0.496) �Ő݌v�����l���g�p

### Option3:
- �����S�҂���A�C�f�A�́uRX�Q�C���ݒ�ƃX�y�N�g�����\���̘A��(ex. RX�Q�C�����グ����M�����x�����オ��)�v����������ꍇ�̕ύX�ӏ�
- radio.c �� static void rfgain_setting( void ) ���̋L�q�����L�ɕύX����( -db+33 �� +13 �ɕύX )
```c
   fft_dboffset( FFT_DBOFFSET+13 );   // �����l�ɍ��킹����
```

- �X�ɃX�y�N�g�����\���̏c���X�P�[��(REF���x��)��A��������ꍇ�́A���L�Ƃ���B
- static void rfgain_setting( void ) ���̋L�q�� speanaref_setting()��ǉ��B
```c
   fft_dboffset( FFT_DBOFFSET+13 );   // �����l�ɍ��킹����
   speanaref_setting();               // �X�y�A�i Ref ��ݒ�
```
speanaref_setting( void ) ���̋L�q�� �X�y�A�iRef��RX�Q�C���ݒ�l����������悤�ɕύX�B
```c
   int sel =  menus[N_MENU_SPREF].sel + menus[N_MENU_RXGAIN].sel - 2 ;
```


### Appendix1:
- �t�^DVD-ROM�̃\�[�X�ɑ΂��āAmain20180812.hex �� setting���j���[�� "Freq Popup"�� "OFF" �ɂ����̂Ɠ�������ɂ��邽�߂̏C���ӏ��B
- radio.c �� radio_rotaryenc ���� 4�ӏ����R�����g�A�E�g����B
```c
   case 0:
     if( (rot>=ACCEL_TH) || (rot<=-ACCEL_TH) ) rot *= ACCEL;
     freq_setting( freq + rot*freqstep );
//   if( popuptime_default ) {
//     gui_popup_show( str_freq );
//   }else{
       gui_update();
//   }
     break;
```

### Appendix2:
- �`���[�j���O����̑�����ݒ�̕ύX
- ���[�^���[�G���R�[�_�̑��񂵔��肵�����l�Ǝ��g�������ʂ̕ύX�ӏ�
- main.h �� ACCEL��ACCEL_TH��ύX����B���L���10�{��
```c
#define ACCEL             (10)          // ���[�^���[�G���R�[�_�[�����W��
#define ACCEL_TH          (2)           // ���[�^���[�G���R�[�_�[�����X���b�V�����h
```
