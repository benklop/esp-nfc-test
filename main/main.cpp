#include <esp_log.h>
#include <MFRC522.h>
#include <NfcAdapter.h>
#include <NdefMessage.h>
#include <NdefRecord.h>
#include <string.h>
#include <algorithm>
#include <iterator>

extern "C" {
	void app_main();
}

static const char LOG_TAG[] = "DumpInfo";
MFRC522 mfrc522;  // Create MFRC522 instance
NfcAdapter nfc = NfcAdapter(&mfrc522);

void dumpInfo() {
	mfrc522.PCD_Init();		// Init MFRC522
	mfrc522.PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details
	ESP_LOGI(LOG_TAG, "Scan PICC to see UID, SAK, type, and data blocks...");

  byte last_uid[10];

	while(1) {
	
  /*
  	// Look for new cards
		if ( ! mfrc522.PICC_IsNewCardPresent()) {
			continue;
		}
		// Select one of the cards
		if ( ! mfrc522.PICC_ReadCardSerial()) {
			continue;
		}

    byte this_uid[10];
    memcpy(this_uid, mfrc522.uid.uidByte, 10);

    if(std::equal(std::begin(this_uid), std::end(this_uid), std::begin(last_uid))) {
      continue;
    }

    memcpy(last_uid, this_uid, 10);
    
		// Dump debug info about the card; PICC_HaltA() is automatically called
		//mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid));
*/
    if (nfc.tagPresent())
    {

      byte this_uid[10];
      memcpy(this_uid, mfrc522.uid.uidByte, 10);

      if(std::equal(std::begin(this_uid), std::end(this_uid), std::begin(last_uid))) {
        continue;
      }

      memcpy(last_uid, this_uid, 10);

      NfcTag tag = nfc.read();
      ESP_LOGI(LOG_TAG, "Scanned Tag type: %d", tag.getTagType());

      if (tag.hasNdefMessage()) // every tag won't have a message
      {
        NdefMessage message = tag.getNdefMessage();
        ESP_LOGI(LOG_TAG, "This NFC Tag contains an NDEF Message with %d NDEF Record%s", message.getRecordCount(), (message.getRecordCount() != 1) ? "s." : ".");

        int recordCount = message.getRecordCount();
        for (int i = 0; i < recordCount; i++)
        {
          ESP_LOGI(LOG_TAG, "NDEF Record %d", i+1);
          NdefRecord record = message.getRecord(i);
          ESP_LOGI(LOG_TAG, "  TNF: %d", record.getTnf());
          //ESP_LOGI(LOG_TAG, "  Type:");
          //ESP_LOG_BUFFER_HEX(LOG_TAG, record.getType(), record.getTypeLength());

          ESP_LOG_BUFFER_HEXDUMP(LOG_TAG, record.getPayload(), record.getPayloadLength(), ESP_LOG_INFO);

          if (record.getIdLength() > 0) {
            ESP_LOGI(LOG_TAG, "ID:");
            ESP_LOG_BUFFER_HEX(LOG_TAG, record.getId(), record.getIdLength());
          }
        }
      }
    }
  }
}

void app_main(void) {
    esp_log_level_set("MFRC522", ESP_LOG_DEBUG);
    esp_log_level_set("DumpInfo", ESP_LOG_DEBUG);
    dumpInfo();
}