# GenIO
GenIO is a contraction of "Generic" and "I/O". It provides a C-style tokenizing parser, a
formatted text output stream, and block-based binary storage streams that make it trivial
to create data file formats that are both forwardly and backwardly compatible.

To read, you might do something like:

ERRCODE BaseObject::Load(genio::IInputStream *is)
{
	if (is)
	{
		genio::FOURCHARCODE blockid;

		// "Object v0" block
		if (is->BeginBlock('OBJ0'))
		{
			// Enumerate all blocks until we reach the terminator block
			while ((blockid = is->NextBlockId()) != genio::IStream::ENDBLOCKID)
			{
				// Validation
				is->BeginBlock(blockid);

				switch (blockid)
				{
					// version 0
					case 'INF0':
					{
						// Read the unique id
						is->ReadUINT64(m_Guid);

						// Read the flags
						UINT32 tmpflags;
						is->ReadUINT32(tmpflags);
						m_Flags.SetAll(tmpflags);

						// ....... more stuff
						
						break;
					}

					case 'INF1':
					{
						UINT16 len;
						is->ReadUINT16(len);

						m_Name.resize(len);
						is->ReadStringW(m_Name.c_str());
						
						break;
					}

					// If it wasn't a recognized block, then it must have been created by
					// a sub-class' SaveCustomBlock function... make sure we load it!
					default:
						LoadCustomBlock(is, blockid);
						break;
				}

				is->EndBlock();
			}

			// We still need to open and close the terminator block, since it was skipped over in the above loop
			is->BeginBlock(genio::IStream::ENDBLOCKID);
			is->EndBlock();

			// Confirmation
			is->EndBlock();
		}

		// Do whatever post-load things we need to do!
		PostLoad();

		return ERR_OK;
	}

	return ERR_NULLSTREAM;
}


To write a stream compatible with the above code, you might do this:

ERRCODE BaseObject::Save(IOutputStream *os, BOOL store_all_props)
{
	if (os)
	{
		if (os->BeginBlock('OBJ0'))
		{
			if (os->BeginBlock('INF0'))
			{
				// Write the unique id
				os->WriteUINT64(m_Guid);

				// Write the flags
				os->WriteUINT32(m_Flags.Get());

				// ............. write other stuff

				os->EndBlock();
			}

			if (os->BeginBlock('INF1'))
			{
				// write the string length
				os->WriteUINT16(m_Name.length());

				// Write the name
				os->WriteStringW(&m_Name);

				os->EndBlock();
			}

			// Save out any custom blocks that derived classes may implement (or that the base class may implement later!)
			SaveCustomBlocks(os);

			// Write a dummy block to close this object's stream out...
			os->BeginBlock(0);
			os->EndBlock();

			os->EndBlock();
		}

		return ERR_OK;
	}

	return ERR_NULLSTREAM;
}

By using the above method, you can always add new data under different blocks and have them
correctly processed - or appropriately ignored.

It is also important to note that blocks can be nested and top-level chunks can be skipped over
if they are unrecognized.

Enjoy!